# GNSS-Shadowing
Predict GNSS Shadowing and HDOP Maps and plan HDOP optimal routes 




## Usage of python bindings


To plan a hdop optimal route:
```
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
```
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
            
            # erster sichtbarer satellite als index into mapper.m_sats:
            print(mapper.m_visibilityMap.getItem(x,y,z)[0]) 
```


To generate a HDOP map as numpy array:
```
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

```
mkdir build
cd build
cmake ..
```

## Build in msys2

```
mkdir build
cd build
cmake .. -G "Unix Makefiles" -DOpenCV_DIR=/mingw64/share/OpenCV/lib
```

There is known bug with boost.python in mingw that causes the python bindings to not build in this platform.

## Howto: Where to put header files?

Headers that should be visible from other projects are put in
the ./<project-name>/include/<project-name> folders.
They need to be added to the following files:
 ./<project-name>/include/<project-name>/CMakeLists.txt
 ./<project-name>/src/CMakeLists.txt

Headers for classes that should not be visible form other projects
should be put in the ./<project-name>/src/ folders.

