#pragma once

#include <boost/python.hpp>
#include <string>

#include "planning/astar.h"
#include "planning/problems.h"

using namespace boost::python;

namespace gnssShadowing {
namespace pythonBindings {


    void bindPlanning()
    {
        class_<planning::MapNode>("MapNode", init<>())
            .def(init<int,int>())
            .def_readwrite("x", &planning::MapNode::x)
            .def_readwrite("y", &planning::MapNode::y)
        ;

        class_<planning::MapTimeNode>("MapTimeNode", init<>())
            .def(init<int,planning::MapNode>())
            .def_readwrite("timestep", &planning::MapTimeNode::first)
            .def_readwrite("position", &planning::MapTimeNode::second)
        ;

        planning::MapNode& (std::vector<planning::MapNode>::*vectorMapNodeAt)(size_t) = &std::vector<planning::MapNode>::at;
        void (std::vector<planning::MapNode>::*vectorMapNodePushback)(const planning::MapNode&) = &std::vector<planning::MapNode>::push_back;
        class_<std::vector<planning::MapNode>>("MapNodeList", init<>())
            .def("at",vectorMapNodeAt,return_value_policy<return_by_value>())
            .def("size",&std::vector<planning::MapNode>::size)
            .def("clear",&std::vector<planning::MapNode>::clear)
            .def("append",vectorMapNodePushback)
        ;

        planning::MapTimeNode& (std::vector<planning::MapTimeNode>::*vectorMapTimeNodeAt)(size_t) = &std::vector<planning::MapTimeNode>::at;
        void (std::vector<planning::MapTimeNode>::*vectorMapTimeNodePushback)(const planning::MapTimeNode&) = &std::vector<planning::MapTimeNode>::push_back;
        class_<std::vector<planning::MapTimeNode>>("MapTimeNodeList", init<>())
            .def("at",vectorMapTimeNodeAt,return_value_policy<return_by_value>())
            .def("size",&std::vector<planning::MapTimeNode>::size)
            .def("clear",&std::vector<planning::MapTimeNode>::clear)
            .def("append",vectorMapTimeNodePushback)
        ;

        class_<planning::MapProblem>("MapProblem", init<float, mapping::DOPMap&, const planning::MapNode&, const planning::MapNode&>(
                            (arg("maxHorizontalDOP")
                            ,arg("map")
                            ,arg("start")
                            ,arg("target")
                            )))
            .def(init<mapping::DOPMap&>())
            .def("computeCost", &planning::MapProblem::computeCost, (arg("from"),arg("to")))
            .def("computeHeuristic", &planning::MapProblem::computeHeuristic, (arg("node")))
            .def("computeNeighbors", &planning::MapProblem::computeNeighbors, return_value_policy<return_by_value>(), (arg("node")))
            .def("targetReached", &planning::MapProblem::targetReached, (arg("node")))
            .def_readwrite("m_maxHorizontalDOP", &planning::MapProblem::m_maxHorizontalDOP)
            .add_property("m_map", make_function(&planning::MapProblem::getMap, return_value_policy<reference_existing_object>()), 
                                    &planning::MapProblem::setMap)
        ;

        class_<planning::MapTimeProblem>("MapTimeProblem", init<float, float, float, float, float, int, 
                        mapping::MapperLazyTimesteps&, const planning::MapTimeNode&, const planning::MapTimeNode&>(
                            (arg("maxHorizontalDOP")
                            ,arg("costPerHorizontalDOP")
                            ,arg("costPerGridStep")
                            ,arg("costPerTimeStep")
                            ,arg("costPerTimeTotal")
                            ,arg("timeStepsPerStep")
                            ,arg("mapperLazyTimesteps")
                            ,arg("start")
                            ,arg("target"))))
            .def("computeCost", &planning::MapTimeProblem::computeCost, (arg("from"),arg("to")))
            .def("computeHeuristic", &planning::MapTimeProblem::computeHeuristic, (arg("node")))
            .def("computeNeighbors", &planning::MapTimeProblem::computeNeighbors, return_value_policy<return_by_value>(), (arg("node")))
            .def("targetReached", &planning::MapTimeProblem::targetReached, (arg("node")))
            .def_readwrite("m_maxHorizontalDOP", &planning::MapTimeProblem::m_maxHorizontalDOP)
            .def_readwrite("m_costPerHorizontalDOP", &planning::MapTimeProblem::m_costPerHorizontalDOP)
            .def_readwrite("m_costPerGridStep", &planning::MapTimeProblem::m_costPerGridStep)
            .def_readwrite("m_costPerTimeStep", &planning::MapTimeProblem::m_costPerTimeStep)
            .def_readwrite("m_costPerTimeTotal", &planning::MapTimeProblem::m_costPerTimeTotal)
            .def_readwrite("m_timeStepsPerStep", &planning::MapTimeProblem::m_timeStepsPerStep)
            .add_property("m_maps", make_function(&planning::MapTimeProblem::getMaps, return_value_policy<reference_existing_object>()), 
                                    &planning::MapTimeProblem::setMaps)
        ;

        std::vector<planning::MapNode> (planning::MapProblemSolver::*findShortestPathMap)(planning::MapProblem&) = &planning::MapProblemSolver::findShortestPath;
        class_<planning::MapProblemSolver>("MapProblemSolver", init<>())
            .def("findShortestPath", findShortestPathMap, return_value_policy<return_by_value>())
        ;

        std::vector<planning::MapTimeNode> (planning::MapTimeProblemSolver::*findShortestPathMapTime)(planning::MapTimeProblem&) = &planning::MapTimeProblemSolver::findShortestPath;
        class_<planning::MapTimeProblemSolver>("MapTimeProblemSolver", init<>())
            .def("findShortestPath", findShortestPathMapTime, return_value_policy<return_by_value>())
        ;

    }

} // namespace pythonBindings
} // namespace gnssShadowing
