
import gnssShadowing
import pytest
import numpy as np

def test_MapNode():
    a=gnssShadowing.MapNode()
    assert a.x == 0
    assert a.y == 0
    assert type(a.x) == int
    assert type(a.y) == int
    a=gnssShadowing.MapNode(1,2)
    assert a.x == 1
    assert a.y == 2
    a.x = 3
    a.y = 4
    assert a.x == 3
    assert a.y == 4

def test_MapTimeNode():
    a=gnssShadowing.MapTimeNode()
    assert a.timestep == 0
    assert a.position.x == 0
    assert a.position.y == 0
    assert type(a.timestep) == int
    assert type(a.position) == gnssShadowing.MapNode

    a=gnssShadowing.MapTimeNode(1,gnssShadowing.MapNode(2,3))
    assert a.timestep == 1
    assert a.position.x == 2
    assert a.position.y == 3
    a.timestep = 4
    a.position.x = 5
    a.position.y = 6
    assert a.timestep == 4
    assert a.position.x == 5
    assert a.position.y == 6

def test_MapNodeList():
    a=gnssShadowing.MapNodeList()
    assert a.size() == 0
    a.append(gnssShadowing.MapNode(1,2))
    assert a.at(0).x == 1
    assert a.at(0).y == 2
    # at returns by value:
    a.at(0).x = 3
    a.at(0).y = 4
    assert a.at(0).x == 1
    assert a.at(0).y == 2
    assert a.size() == 1
    a.append(gnssShadowing.MapNode(5,6))
    b = a.at(1)
    assert b.x == 5
    assert b.y == 6
    assert a.size() == 2
    a.clear()
    assert a.size() == 0

def test_MapTimeNodeList():
    a=gnssShadowing.MapTimeNodeList()
    assert a.size() == 0
    a.append(gnssShadowing.MapTimeNode(1,gnssShadowing.MapNode(2,3)))
    assert a.at(0).timestep == 1
    assert a.at(0).position.x == 2
    assert a.at(0).position.y == 3
    # at returns by value:
    a.at(0).timestep = 4
    a.at(0).position.x = 5
    a.at(0).position.y = 6
    assert a.at(0).timestep == 1
    assert a.at(0).position.x == 2
    assert a.at(0).position.y == 3
    a.append(gnssShadowing.MapTimeNode(7,gnssShadowing.MapNode(8,9)))
    b = a.at(1)
    assert b.timestep == 7
    assert b.position.x == 8
    assert b.position.y == 9
    assert a.size() == 2
    a.clear()
    assert a.size() == 0

def test_MapProblem():
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
    mapper = gnssShadowing.Mapper(world, mapProperties, minimumElevation)
    dopMap = mapper.computeDOPMap(gnssShadowing.mkSeconds(2017,3,28,12,0,0))

    maxHorizontalDOP = 5
    start = gnssShadowing.MapNode(0,0)
    target = gnssShadowing.MapNode(1,1)

    problem = gnssShadowing.MapProblem(maxHorizontalDOP,dopMap,
                                       start,target)

    assert type(problem.m_map) == gnssShadowing.DOPMap
    assert type(problem.m_maxHorizontalDOP) == float
    assert problem.targetReached(target) == True
    assert problem.targetReached(start) == False
    problem.computeCost(start,target)
    neighbors = problem.computeNeighbors(start)
    assert type(neighbors) == gnssShadowing.MapNodeList
    problem.computeHeuristic(start)

def test_MapTimeProblem():
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
    time = gnssShadowing.mkSeconds(2017,3,28,12,0,0)
    timePerStep = 5 # in seconds
    mapper = gnssShadowing.MapperLazyTimesteps(world,mapProperties,time,timePerStep,minimumElevation)

    maxHorizontalDOP = 5
    start  = gnssShadowing.MapTimeNode(0,gnssShadowing.MapNode(0,0))
    target = gnssShadowing.MapTimeNode(0,gnssShadowing.MapNode(1,1))
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
    assert type(problem.m_maps) == gnssShadowing.MapperLazyTimesteps
    assert type(problem.m_maxHorizontalDOP) == float
    assert type(problem.m_costPerHorizontalDOP) == float
    assert type(problem.m_costPerGridStep) == float
    assert type(problem.m_costPerTimeStep) == float
    assert type(problem.m_costPerTimeTotal) == float
    assert type(problem.m_timeStepsPerStep) == int
    assert problem.targetReached(target) == True
    assert problem.targetReached(start) == False
    problem.computeCost(start,target)
    neighbors = problem.computeNeighbors(start)
    assert type(neighbors) == gnssShadowing.MapTimeNodeList
    problem.computeHeuristic(start)


def test_MapProblemSolver():
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
    mapper = gnssShadowing.Mapper(world, mapProperties, minimumElevation)
    dopMap = mapper.computeDOPMap(gnssShadowing.mkSeconds(2017,3,28,12,0,0))

    maxHorizontalDOP = 5
    start = gnssShadowing.MapNode(0,0)
    target = gnssShadowing.MapNode(1,1)

    problem = gnssShadowing.MapProblem(maxHorizontalDOP,dopMap,
                                       start,target)

    solver = gnssShadowing.MapProblemSolver()
    path = solver.findShortestPath(problem)
    assert type(path) == gnssShadowing.MapNodeList
    assert path.size() == 3
    assert path.at(0).x == start.x
    assert path.at(0).y == start.y
    assert path.at(path.size()-1).x == target.x
    assert path.at(path.size()-1).y == target.y

def test_MapTimeProblemSolver():
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
    time = gnssShadowing.mkSeconds(2017,3,28,12,0,0)
    timePerStep = 5 # in seconds
    mapper = gnssShadowing.MapperLazyTimesteps(world,mapProperties,time,timePerStep,minimumElevation)

    maxHorizontalDOP = 5
    start  = gnssShadowing.MapTimeNode(0,gnssShadowing.MapNode(0,0))
    target = gnssShadowing.MapTimeNode(0,gnssShadowing.MapNode(1,1))
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
    assert type(path) == gnssShadowing.MapTimeNodeList
    assert path.size() == 3
    assert path.at(0).position.x == start.position.x
    assert path.at(0).position.y == start.position.y
    assert path.at(path.size()-1).position.x == target.position.x
    assert path.at(path.size()-1).position.y == target.position.y
