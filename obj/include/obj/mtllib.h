#pragma once

#include <string>
#include <sstream>
#include <functional>
#include <unordered_map>
#include <opencv2/opencv.hpp>

namespace gnssShadowing {
namespace obj {

    struct Material
    {
        std::string name;
        cv::Point3f ambient;
        cv::Point3f diffuse;
    };

    class Mtllib
    {
    public:
        Mtllib();
        ~Mtllib() = default;
        void purge();
        void read(const std::string& filename);
        
        Material& getMaterial(const std::string& name) {return m_materials[name];};
        std::unordered_map<std::string, Material> m_materials;
    protected:
        std::string m_currentMaterialName;
        void readStatement_newmtl(std::string wholeLine, std::istringstream& iss);
        void readStatement_Ka(std::string wholeLine, std::istringstream& iss);
        void readStatement_Kd(std::string wholeLine, std::istringstream& iss);
    };
    
    std::ostream& operator<<(std::ostream&, const Mtllib&);

    typedef std::function<void(std::string,std::istringstream&)> MtllibStatementReaderFunction;

} // namespace obj
} // namespace gnssShadowing

