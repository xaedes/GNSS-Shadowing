#include "mapping/mapProperties.h"

namespace gnssShadowing {
namespace mapping {

    std::ostream& operator<<(std::ostream& os, const MapProperties& mapProperties)
    {
        os << "["
            "min_x: " << mapProperties.m_min_x << " "
            "min_y: " << mapProperties.m_min_y << " "
            "num_cols: " << mapProperties.m_num_cols << " "
            "num_rows: " << mapProperties.m_num_rows << " "
            "x_resolution: " << mapProperties.m_x_resolution << " "
            "y_resolution: " << mapProperties.m_y_resolution << " "
            "plane_levels: ";
        bool first = true;
        for (double planeLevel : mapProperties.m_plane_levels)
        {
            if (!first) os << "; ";
            first = false;
            os << planeLevel;
        }
        os << "]" << std::endl;
        return os;
    }

} // namespace mapping
} // namespace gnssShadowing
