
namespace gnssShadowing {
namespace mapping {

    template <typename Item>
    VectorMap<Item>::VectorMap(const MapProperties& properties) :
        m_properties(properties)
    {
        initMaps();
    }

    template <typename Item>
    void VectorMap<Item>::initMaps()
    {
        m_items.clear();

        for (int k=0; k < m_properties.m_plane_levels.size(); k++)
        {
            std::vector<std::vector<Item>> map;
            for (int x=0; x < m_properties.m_num_cols; x++)
            {
                std::vector<Item> column;
                for (int y=0; y < m_properties.m_num_rows; y++)
                {
                    Item item;
                    column.push_back(item);
                }
                map.push_back(column);
            }
            m_items.push_back(map);
        }

    }


} // namespace mapping
} // namespace gnssShadowing
