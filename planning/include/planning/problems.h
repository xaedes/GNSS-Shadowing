#pragma once
#include <ostream>
#include <opencv2/opencv.hpp>
#include "mapping/dopMap.h"
#include "planning/astar.h"
#include "mapping/mapperLazyTimesteps.h"
#include "mapping/occupancyMap.h"

namespace gnssShadowing {
namespace planning {

    typedef cv::Point2i MapNode;

    struct MapNodeHash
    {
        size_t operator()(const MapNode &key) const { 
            return std::hash<int>()(key.x) ^ std::hash<int>()(key.y);
        }
    };
    struct MapNodeEqual
    {
        bool operator()(const MapNode &lhs, const MapNode &rhs) const{
            return lhs.x == rhs.x && lhs.y == rhs.y;
        }
    };
    class MapProblem : public PathplanningProblem<MapNode>
    {
    public:
        MapProblem(float maxHorizontalDOP, mapping::DOPMap& map, const MapNode& start, const MapNode& target) : 
            PathplanningProblem<MapNode>(start, target),
            m_maxHorizontalDOP(maxHorizontalDOP),
            m_map(map)
        {}
        float computeCost(const MapNode& from, const MapNode& to);
        float computeHeuristic(const MapNode& node);
        std::vector<MapNode> computeNeighbors(const MapNode& node);

        MapProblem(mapping::DOPMap& map) : m_map(map) {}
        float m_maxHorizontalDOP;
        mapping::DOPMap& m_map;

        mapping::DOPMap& getMap() {return m_map;}
        void setMap(mapping::DOPMap& map) {m_map=map;}

    protected:
        void addIf(std::vector<MapNode>& results, const MapNode& node);
    };
    typedef AStarSolver<MapNode,MapProblem,MapNodeHash,MapNodeEqual> MapProblemSolver;



    typedef int TimeStep;
    typedef std::pair<TimeStep,MapNode> MapTimeNode;
    struct MapTimeNodeHash
    {
        size_t operator()(const std::pair<TimeStep, MapNode> &key) const { 
            return std::hash<TimeStep>()(key.first) ^ std::hash<int>()(key.second.x) ^ std::hash<int>()(key.second.y);
        }
    };
    struct MapTimeNodeEqual
    {
        bool operator()(const std::pair<TimeStep, MapNode> &lhs, const std::pair<TimeStep, MapNode> &rhs) const{
            return lhs.first == rhs.first && lhs.second == rhs.second;
        }
    };
    // ragarding MapTimeNodeHash,MapTimeNodeEqual: http://stackoverflow.com/q/33597127/798588
    std::ostream& operator<<(std::ostream&, const MapTimeNode&);
    class MapTimeProblem : public PathplanningProblem<MapTimeNode>
    {
    public:
        MapTimeProblem(float maxHorizontalDOP, float costPerHorizontalDOP, float costPerGridStep, float costPerTimeStep, float costPerTimeTotal, int timeStepsPerStep, 
                        mapping::MapperLazyTimesteps& maps, const MapTimeNode& start, const MapTimeNode& target) : 
            PathplanningProblem<MapTimeNode>(start, target),
            m_maps(maps),
            m_maxHorizontalDOP(maxHorizontalDOP),
            m_costPerHorizontalDOP(costPerHorizontalDOP),
            m_costPerGridStep(costPerGridStep),
            m_costPerTimeStep(costPerTimeStep),
            m_costPerTimeTotal(costPerTimeTotal),
            m_timeStepsPerStep(timeStepsPerStep)
        {}
        float computeCost(const MapTimeNode& from, const MapTimeNode& to);
        float computeHeuristic(const MapTimeNode& node);
        std::vector<MapTimeNode> computeNeighbors(const MapTimeNode& node);
        bool targetReached(const MapTimeNode& node);

        mapping::MapperLazyTimesteps& getMaps() {return m_maps;}
        void setMaps(mapping::MapperLazyTimesteps& maps) {m_maps=maps;}
        mapping::MapperLazyTimesteps& m_maps;
        float m_maxHorizontalDOP;
        float m_costPerHorizontalDOP;
        float m_costPerGridStep;
        float m_costPerTimeStep;
        float m_costPerTimeTotal;
        int m_timeStepsPerStep;

    protected:
        void addIf(std::vector<MapTimeNode>& results, const MapTimeNode& node);
    };
    typedef AStarSolver<MapTimeNode,MapTimeProblem,MapTimeNodeHash,MapTimeNodeEqual> MapTimeProblemSolver;

} // namespace planning
} // namespace gnssShadowing
