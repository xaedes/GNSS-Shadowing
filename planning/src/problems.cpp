#include "planning/problems.h"

#include <iostream>
using namespace std;

namespace gnssShadowing {
namespace planning {

    /**
     * MapProblem
     */

    void MapProblem::addIf(std::vector<MapNode>& results, const MapNode& node)
    {
        if (m_map.m_items[0][node.x][node.y].getHorizontal() < m_maxHorizontalDOP)
        {
            results.push_back(node);
        }
    }

    std::vector<MapNode> MapProblem::computeNeighbors(const MapNode& node)
    {
        int w = m_map.m_properties.m_num_cols;
        int h = m_map.m_properties.m_num_rows;
        std::vector<MapNode> neighbors;
        if (node.x > 0)   addIf(neighbors, MapNode(node.x-1,node.y  ));
        if (node.y > 0)   addIf(neighbors, MapNode(node.x,  node.y-1));
        if (node.x < w-1) addIf(neighbors, MapNode(node.x+1,node.y  ));
        if (node.y < h-1) addIf(neighbors, MapNode(node.x,  node.y+1));
        return neighbors;
    }

    float MapProblem::computeCost(const MapNode& from, const MapNode& to)
    {
        return cv::norm(to-from);
    }

    float MapProblem::computeHeuristic(const MapNode& node)
    {
        return cv::norm(m_target-node);
    }

    /**
     * MapTimeProblem
     */
    void MapTimeProblem::addIf(std::vector<MapTimeNode>& results, const MapTimeNode& node)
    {
        float dop = m_maps.getDOPMap(node.first).m_items[0][node.second.x][node.second.y].getHorizontal();
        if (dop<0) dop = 50;
        bool occupied = m_maps.getOccupancyMap(node.first).isOccupied(node.second.x,node.second.y,0);
        if ((dop < m_maxHorizontalDOP) && !occupied)
        {
            results.push_back(node);
        }
    }

    std::vector<MapTimeNode> MapTimeProblem::computeNeighbors(const MapTimeNode& node)
    {
        int w = m_maps.getDOPMap(node.first).m_properties.m_num_cols;
        int h = m_maps.getDOPMap(node.first).m_properties.m_num_rows;
        std::vector<MapTimeNode> neighbors;
        addIf(neighbors, MapTimeNode(node.first+m_timeStepsPerStep,MapNode(node.second.x,node.second.y)));
        if (node.second.x > 0)   addIf(neighbors, MapTimeNode(node.first+m_timeStepsPerStep,MapNode(node.second.x-1,node.second.y  )));
        if (node.second.y > 0)   addIf(neighbors, MapTimeNode(node.first+m_timeStepsPerStep,MapNode(node.second.x,  node.second.y-1)));
        if (node.second.x < w-1) addIf(neighbors, MapTimeNode(node.first+m_timeStepsPerStep,MapNode(node.second.x+1,node.second.y  )));
        if (node.second.y < h-1) addIf(neighbors, MapTimeNode(node.first+m_timeStepsPerStep,MapNode(node.second.x,  node.second.y+1)));
        return neighbors;
    }

    float MapTimeProblem::computeCost(const MapTimeNode& from, const MapTimeNode& to)
    {
        float dop = m_maps.getDOPMap(to.first).m_items[0][to.second.x][to.second.y].getHorizontal();
        float cost =   m_costPerGridStep * cv::norm(to.second-from.second) 
                         + m_costPerHorizontalDOP * dop
                         + m_costPerTimeStep * (to.first - from.first)
                         + m_costPerTimeTotal * to.first;
        return cost;
    }

    float MapTimeProblem::computeHeuristic(const MapTimeNode& node)
    {
        cv::Vec2i diff = m_target.second-node.second;
        float distance = cv::norm(diff);
        float heuristic = m_costPerGridStep * distance;
        // discarding other (always positive) error terms does not invalidate heuristic condition
        
         
        if (m_costPerHorizontalDOP < 0)
        {
            // heuristic condition may be invalidated, i.e. heuristic cost may be higher than actual cost
            // reduce value by a guess of the expected maximal negative cost term
            int approxMaxDiff = diff[0] + diff[1];
            heuristic += m_costPerHorizontalDOP * approxMaxDiff;
        }
        return heuristic;
    }
    bool MapTimeProblem::targetReached(const MapTimeNode& node)
    {
        return node.second == m_target.second;
    }
    std::ostream& operator<<(std::ostream& stream, const MapTimeNode& node)
    {
        stream << "timestep: " << node.first << " "
             << "x: " << node.second.x << " "
             << "y: " << node.second.y;

        return stream;
    }

} // namespace planning
} // namespace gnssShadowing
