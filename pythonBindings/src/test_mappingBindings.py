
import gnssShadowing
import pytest
import numpy as np
import numpy.testing as nt

def test_PlaneLevelList():
    planeLevels = gnssShadowing.PlaneLevelList()
    planeLevels.append(1)
    planeLevels.append(2)
    planeLevels.append(3)
    assert planeLevels[0] == 1
    assert planeLevels[1] == 2
    assert planeLevels[2] == 3


def test_MapProperties():
    planeLevels = gnssShadowing.PlaneLevelList()
    planeLevels.append(1.0)
    mapProperties = gnssShadowing.MapProperties()
    min_x = 10.0
    min_y = 20.0
    width = 5
    height = 3
    res_x = res_y = 2.0
    mapProperties = gnssShadowing.MapProperties(min_x,min_y,width,height,res_x,res_y,planeLevels)
    assert mapProperties.m_min_x == min_x
    assert mapProperties.m_min_y == min_y
    assert mapProperties.m_num_cols == width
    assert mapProperties.m_num_rows == height
    assert mapProperties.m_x_resolution == res_x
    assert mapProperties.m_y_resolution == res_y
    assert mapProperties.m_plane_levels[0] == planeLevels[0]

    assert mapProperties.X(0) == min_x
    assert mapProperties.Y(0) == min_y
    assert mapProperties.X(width-1) == min_x + (width-1)*res_x
    assert mapProperties.Y(height-1) == min_y + (height-1)*res_y

    assert 0 == mapProperties.I(min_x)
    assert 0 == mapProperties.J(min_y)
    assert width-1 == mapProperties.I(min_x + (width-1)*res_x)
    assert height-1 == mapProperties.J(min_y + (height-1)*res_y)

def test_ShadowMap():
    world=gnssShadowing.World("data/2017-03-28.tle","data/uni.obj","Building")
    shadowing=world.m_shadowing
    now=gnssShadowing.mkSeconds(2017,3,28,12,0,0)

    idx = 57
    horizontal=world.m_satStore.getSat(idx).findECEF(now).toENU(world.m_origin).toHorizontal()
    transformation=shadowing.computeTransformation(horizontal)
    contour=shadowing.computeContourFromTransformation(transformation)
    planeLevel=0
    eps=1e-3
    vol=shadowing.computeShadowVolume(contour,transformation,planeLevel,eps)
    planeLevels = gnssShadowing.PlaneLevelList()
    planeLevels.extend([56.0])
    min_x = 10.0
    min_y = 20.0
    width = 5
    height = 3
    res_x = res_y = 2.0
    
    mapProperties = gnssShadowing.MapProperties(min_x,min_y,width,height,res_x,res_y,planeLevels)

    shadowMap1=gnssShadowing.ShadowMap(mapProperties)
    assert type(shadowMap1.m_properties) == gnssShadowing.MapProperties
    z = 0 # planeLevelIndex
    for x in range(0,width):
        for y in range(0,height):
            assert shadowMap1.isShadowed(x,y,z) == False
    map1=shadowMap1.getMapForLevel(0)
    assert np.sum(map1!=0) == 0
    shadowMap2=gnssShadowing.ShadowMap(mapProperties,vol)
    map2=shadowMap2.getMapForLevel(0)
    
    # the reference value (11) was computed for sat #57 in "data/2017-03-28.tle","data/uni.obj" on commit 23e15ee656dcaf805d59ab68965718800f29f1fa
    assert np.sum(map2!=0) == 11 

def test_Mapper():
    world=gnssShadowing.World("data/2017-03-28.tle","data/uni.obj","Building")
    planeLevels = gnssShadowing.PlaneLevelList()
    planeLevels.extend([56.0])
    min_x = 10.0
    min_y = 20.0
    width = 5
    height = 3
    res_x = res_y = 2.0
    mapProperties = gnssShadowing.MapProperties(min_x,min_y,width,height,res_x,res_y,planeLevels)
    minimumElevation = gnssShadowing.deg2rad(5)
    mapper = gnssShadowing.Mapper(world, mapProperties, minimumElevation)
    assert type(mapper.m_world) == gnssShadowing.World
    assert type(mapper.m_sats) == gnssShadowing.AvailableSats
    assert type(mapper.m_mapProperties) == gnssShadowing.MapProperties
    assert type(mapper.m_occupancyMap) == gnssShadowing.OccupancyMap
    assert type(mapper.m_visibilityMap) == gnssShadowing.VisibilityMap
    assert type(mapper.m_dopMap) == gnssShadowing.DOPMap

    # not initialized yet
    assert mapper.m_sats.getNumSats() == 0

    mapper.updateSats(gnssShadowing.mkSeconds(2017,3,28,12,0,0))
    assert mapper.m_sats.getNumSats() == 21

    # not initialized yet
    sumVisible = 0
    z = 0
    for x in range(0,width):
        for y in range(0,height):
            sumVisible+=mapper.m_visibilityMap.getItem(x,y,z).size()
    assert sumVisible == 0

    # not initialized yet
    hdop = mapper.m_dopMap.getHDOPMap(z)
    sumNotInvalidHDOP = 0
    for x in range(0,width):
        for y in range(0,height):
            if mapper.m_dopMap.getItem(x,y,z).getHorizontal() != -1:
                sumNotInvalidHDOP += 1
    assert np.sum(hdop != -1) == 0
    assert sumNotInvalidHDOP == 0

def test_OccupancyMap():
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
    occupancyMap = mapper.m_occupancyMap
    z = 0 # planeLevelIndex
    sumOccupied = 0
    for x in range(0,width):
        for y in range(0,height):
            if occupancyMap.isOccupied(x,y,z):
                sumOccupied+=1
    assert sumOccupied == 7557
    npMap = occupancyMap.getMapForLevel(z)
    assert type(npMap) == np.ndarray
    assert npMap.sum() == sumOccupied

def test_VisibilityMap():
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
    assert sumVisible == 1529510

def test_DOPMap():
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
    sumNotInvalidHDOP = 0
    for x in range(0,width):
        for y in range(0,height):
            if mapper.m_dopMap.getItem(x,y,z).getHorizontal() != -1:
                sumNotInvalidHDOP += 1
    assert np.sum(hdop != -1) == 79859
    assert sumNotInvalidHDOP == 79859

    hdop = dopMap.getHDOPMap(z)
    sumNotInvalidHDOP = 0
    for x in range(0,width):
        for y in range(0,height):
            if dopMap.getItem(x,y,z).getHorizontal() != -1:
                sumNotInvalidHDOP += 1
    assert np.sum(hdop != -1) == 79859
    assert sumNotInvalidHDOP == 79859

    sumHDOP = 0
    for x in range(0,width):
        for y in range(0,height):
            sumHDOP += mapper.m_dopMap.getItem(x,y,z).getHorizontal()
    assert int(sumHDOP*10) == 526101
    assert int(np.sum(hdop)) == int(sumHDOP)
    
def test_MapCoordinateConverter():
    world=gnssShadowing.World("data/2017-03-28.tle","data/uni.obj","Building")
    planeLevels = gnssShadowing.PlaneLevelList()
    planeLevels.extend([56.0])
    width_m = 10
    height_m = 10
    res_x = res_y = 1.0
    width = (int)(width_m / res_x)
    height = (int)(height_m / res_y)
    min_x = -width_m/2.
    min_y = -height_m/2.
    mapProperties = gnssShadowing.MapProperties(min_x,min_y,width,height,res_x,res_y,planeLevels)

    conv = gnssShadowing.MapCoordinateConverter(mapProperties,world.m_origin)
    a = conv.gridCoordinateToGeodetic(gnssShadowing.MapNode(0,0))
    b = conv.geodeticToGridCoordinate(a)
    c = conv.geodeticToGridCoordinateF(a)
    assert type(a) == gnssShadowing.Geodetic
    assert type(b) == gnssShadowing.MapNode
    assert type(c) == gnssShadowing.MapNodeF
