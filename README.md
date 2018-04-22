# GNSS-Shadowing
Predict GNSS Shadowing and HDOP Maps and plan HDOP optimal routes 

Written in C++ and has a working python bindings. 

The ideas behind it are simple: Satellite trajectories are computed using SGP4 from publicly available TLE files. The current ones will be downloaded from cmake. Some coordinate system transformations later a simple software shadow rasterization (in layered 2D map) algorithm is performed with 3D data pulled in from .obj files that are generated from OpenStreetMap / OSM2World. Dilution of Precision values are calculated from visible satellites for each rasterization point. The shadow information could also be used to do shadow matching.

## Acquiring 3D world data

We use freely available data from OpenStreetMap and the tool OSM2World to extract .obj files.

Go to OSM2World Webpage http://osm2world.org/ and download OSM2World tool.

SRTM data is used to get terrain height. This can be downloaded from: https://dds.cr.usgs.gov/srtm/version2_1/SRTM3
The files are grouped by continents and integer valued (truncated) longitudes and latitudes.
E.g. for the test area during the development (52.138749,11.639704 - 52.141384,11.646782) the filename would be N25E011.hgt.zip. The continent would be Eurasia.
So the complete url would be https://dds.cr.usgs.gov/srtm/version2_1/SRTM3/Eurasia/N52E011.hgt.zip
This is a zip file that contains the SRTM file with the terrain height information.
To use this in OSM2World put the extracted SRTM file in a "srtm" subfolder of the OSM2World directory.

OSM2World isn't configured by default to use SRTM data. So you need to create a config file from the example "config.properties" that comes with OSM2World. There is a line that specifies the srtm data folder to use. Uncomment this so the following setting is set:
srtmDir = srtm

Now OSM2World can be invoked from command line with the newly created config file as parameter.
Additionally the longitude,latitude limits for the extracted world patch need to be specified:

```
(osm-program) --config example_config.properties
--output result.obj \
--input_bbox 52.138749,11.639704 \
52.141384,11.646782 \
--input_mode OVERPASS
```

(osm-program) is "osm2world-windows.bat" for Windows and osm2world.sh for Linux.

The resulting file "result.obj" can now be used for GNSS shadowing.


## Usage of python bindings


To plan a hdop optimal route:
```python
import gnssShadowing
world=gnssShadowing.World("data/2017-03-28.tle","data/uni.obj","Building")
planeLevels = gnssShadowing.PlaneLevelList()
planeLevels.extend([56.0])
width_m = 800
height_m = 400
res_x = res_y = 10.0
width = (int)(width_m / res_x)
height = (int)(height_m / res_y)
min_x = -width_m/2.
min_y = -height_m/2.
mapProperties = gnssShadowing.MapProperties(min_x,min_y,width,height,res_x,res_y,planeLevels)
minimumElevation = gnssShadowing.deg2rad(5)
time = gnssShadowing.mkSeconds(2017,3,28,12,0,0) # oder gnssShadowing.unixTimeInSeconds()
timePerStep = 5 # in seconds
mapper = gnssShadowing.MapperLazyTimesteps(world,mapProperties,time,timePerStep,minimumElevation)

maxHorizontalDOP = 5
conv = gnssShadowing.MapCoordinateConverter(mapProperties,world.m_origin)
# latitude,longitude in gitterkoordinaten umwandeln:
d2r = gnssShadowing.deg2rad
start_geo = gnssShadowing.Geodetic(d2r(52.140992), d2r(11.640801), planeLevels[0],0)
target_geo = gnssShadowing.Geodetic(d2r(52.139854), d2r(11.645191), planeLevels[0],0)
start = gnssShadowing.MapTimeNode(0,conv.geodeticToGridCoordinate(start_geo))
target = gnssShadowing.MapTimeNode(0,conv.geodeticToGridCoordinate(target_geo))
# oder direkt:
#start  = gnssShadowing.MapTimeNode(0,gnssShadowing.MapNode(0,0))
#target = gnssShadowing.MapTimeNode(0,gnssShadowing.MapNode(1,1))
maxHorizontalDOP = 5.
costPerHorizontalDOP = 1.
costPerGridStep = 1.
costPerTimeStep = 1.
costPerTimeTotal = 0.
timeStepsPerStep = 1
problem = gnssShadowing.MapTimeProblem(
        maxHorizontalDOP,costPerHorizontalDOP,costPerGridStep,
        costPerTimeStep,costPerTimeTotal,timeStepsPerStep,
        mapper, 
        start,target)

solver = gnssShadowing.MapTimeProblemSolver()
path = solver.findShortestPath(problem)
```

To generate a visibility map and access its elements:
```python
import gnssShadowing
world=gnssShadowing.World("data/2017-03-28.tle","data/uni.obj","Building")
planeLevels = gnssShadowing.PlaneLevelList()
planeLevels.extend([56.0])
width_m = 800
height_m = 400
res_x = res_y = 2.0
width = (int)(width_m / res_x)
height = (int)(height_m / res_y)
min_x = -width_m/2.
min_y = -height_m/2.
mapProperties = gnssShadowing.MapProperties(min_x,min_y,width,height,res_x,res_y,planeLevels)
minimumElevation = gnssShadowing.deg2rad(5)
mapper = gnssShadowing.Mapper(world, mapProperties, minimumElevation)
mapper.computeDOPMap(gnssShadowing.mkSeconds(2017,3,28,12,0,0))
visibilityMap = mapper.m_visibilityMap
sumVisible = 0
z = 0
for x in range(0,width):
    for y in range(0,height):
        sumVisible+=mapper.m_visibilityMap.getItem(x,y,z).size()
        
        # first visible satellite as index into mapper.m_sats:
        print(mapper.m_visibilityMap.getItem(x,y,z)[0]) 
```


To generate a HDOP map as numpy array:
```python
import gnssShadowing
world=gnssShadowing.World("data/2017-03-28.tle","data/uni.obj","Building")
planeLevels = gnssShadowing.PlaneLevelList()
planeLevels.extend([56.0])
width_m = 800
height_m = 400
res_x = res_y = 2.0
width = (int)(width_m / res_x)
height = (int)(height_m / res_y)
min_x = -width_m/2.
min_y = -height_m/2.
mapProperties = gnssShadowing.MapProperties(min_x,min_y,width,height,res_x,res_y,planeLevels)
minimumElevation = gnssShadowing.deg2rad(5)
mapper = gnssShadowing.Mapper(world, mapProperties, minimumElevation)
dopMap = mapper.computeDOPMap(gnssShadowing.mkSeconds(2017,3,28,12,0,0))
z=0
hdop = mapper.m_dopMap.getHDOPMap(z)
assert type(hdop) == np.ndarray
```

## Build

```shell
mkdir build
cd build
cmake ..
```

## Build in msys2

```shell
mkdir build
cd build
cmake .. -G "Unix Makefiles" -DOpenCV_DIR=/mingw64/share/OpenCV/lib
```

There is known bug with boost.python in mingw that causes the python bindings to not build in this platform.

## Dependencies

### dev tools
```shell
sudo apt-get install -y git cmake htop binutils build-essential pkg-config gdb valgrind
```
### for compiling qt stuff
```shell
sudo apt-get install -y qt5-default libqt4-opengl-dev libqt5opengl5-dev
```
### for compiling boost stuff
```shell
sudo apt-get install -y libboost-all-dev
```    
### for compiling opengl stuff
```shell
sudo apt-get install -y freeglut3 freeglut3-dev libgl1-mesa-dev libglew-dev libglu1-mesa-dev  mesa-common-dev libxmu-dev libxi-dev 
```

### OpenCV3 

## Howto: Where to put header files?

Headers that should be visible from other projects are put in
the `./project-name/include/project-name` folders.
They need to be added to the following files:
- `./project-name/include/project-name/CMakeLists.txt`
- `./project-name/src/CMakeLists.txt`

Headers for classes that should not be visible form other projects
should be put in the `./project-name/src/` folders.

