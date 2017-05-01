#pragma once

#include <vector>
#include <utility>

namespace gnssShadowing {
namespace planning {

    // typedef cv::Vec2i Node;
    // typedef float Cost;


    template <typename Node>
    class PathplanningProblem
    {
    public:
        PathplanningProblem(const Node& start, const Node& target) : 
            m_start(start),
            m_target(target)
        {}
        virtual float computeCost(const Node& from, const Node& to) = 0;
        virtual float computeHeuristic(const Node& node) = 0;
        virtual std::vector<Node> computeNeighbors(const Node& node) = 0;
        virtual bool targetReached(const Node& node) { return m_target == node; };

        PathplanningProblem() {}
        Node m_start;
        Node m_target;
    };


    template <typename Node, typename Problem, class NodeHash, class NodeEqual>
    class AStarSolver
    {
    public:
        
        AStarSolver(){}

        std::vector<Node> findShortestPath(Problem& problem);
    };

} // namespace planning
} // namespace gnssShadowing

#include "planning/astar.h.cpp"
