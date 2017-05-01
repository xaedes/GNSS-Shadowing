// #include "planning/astar.h"

#include <queue>
#include <stdint.h> // uint16_t etc.
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <iostream>

namespace gnssShadowing {
namespace planning {

    template <typename T1, typename T2>
    struct LessFirst
    {
        inline bool operator() (const std::pair<T1,T2>& item1, const std::pair<T1,T2>& item2)
        {
            return (item1.first > item2.first);
        }
    };



    template <typename Node,typename Problem, class NodeHash, class NodeEqual>
    std::vector<Node> AStarSolver<Node,Problem,NodeHash,NodeEqual>::findShortestPath(Problem& problem)
    {
        typedef float Cost;
        typedef std::pair<Cost,Node> Item;

        const Node& start = problem.m_start;
        
        std::unordered_set<Node,NodeHash,NodeEqual> closed;
        std::unordered_map<Node,Node,NodeHash,NodeEqual> predecessors;
        std::unordered_map<Node,Cost,NodeHash,NodeEqual> costs;
        LessFirst<float,Node> compare;
        std::priority_queue<Item, std::vector<Item>, decltype(compare)> openList(compare);

        openList.push(Item(0,start));
        while(!openList.empty())
        {
            Item item = openList.top();
            openList.pop();
            Node& node = item.second;
            if (problem.targetReached(node))
            {
                // goal reached

                // collect path from target to start
                Node current = node;
                std::vector<Node> reversedPath;
                while (current != start)
                {
                    reversedPath.push_back(current);
                    current = predecessors[current];
                }
                reversedPath.push_back(current);

                // reverse the path so it is ordered 
                // from start to target
                std::vector<Node> result;
                for (int k=reversedPath.size()-1;k>=0;k--)
                {
                    const Node& node = reversedPath[k];
                    result.push_back(node);
                }
                return result;
                break;
            }
            if (closed.count(node))
            {
                // node is already visited:
                // The current item from the priority queue openList
                // is an old expansion from one of its neighbors
                // that got selected before the current one, i.e.
                // its cost was better.
                // Old items like the current with higher (worse) cost
                // are kept in queue until they are discarded here.
                // This is an easy way to solve A*star with STL priority 
                // queue that lacks a decrementKey function.
                // see http://stackoverflow.com/a/27305600/798588
                continue;
            } 
            closed.insert(node);
            std::vector<Node> neighbors = problem.computeNeighbors(node);
            for (Node& neighbor : neighbors)
            {
                if (closed.count(neighbor)) continue;
                
                float cost = costs[node] + problem.computeCost(node, neighbor);
                
                if (costs.count(neighbor) && cost > costs[neighbor])
                {
                    // neighbor already expanded from elsewhere but
                    // the current expansion has worse costs
                    continue;
                }

                predecessors[neighbor] = node;
                costs[neighbor] = cost;

                openList.push(Item(cost+problem.computeHeuristic(neighbor),neighbor));
            }
        }
        std::cout << "target not found" << std::endl;
        std::vector<Node> result;
        return result;
    }

} // namespace planning
} // namespace gnssShadowing
