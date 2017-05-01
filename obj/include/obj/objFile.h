#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <functional>
#include <set>
#include <unordered_map>
#include <opencv2/opencv.hpp>

#include "obj/mtllib.h"

namespace gnssShadowing {
namespace obj {

    class ObjFile;

    struct Face
    {
        ObjFile* objFile;
        std::vector<int> vertexIndices;
        std::string materialName;
    };
    
    struct Object
    {
        ObjFile* objFile;
        std::string name;
        std::set<int> vertexIndices;
        std::vector<Face> faces;
        std::unordered_map<std::string, std::vector<Face> > facesByMaterial;
    };

    struct Group : public Object
    {
        std::vector<Object*> objects;
    };

    struct Projection
    {
        Projection():valid(false){}
        bool valid;
        float latitude;
        float longitude;
        float elevation;
        cv::Point3f northDirection;
    };
    std::istream& operator>> (std::istream &in, Projection &projection);

    class ObjFile
    {
    public:
        ObjFile(const std::string& filename);
        ~ObjFile() = default;
        void purge();
        void read(const std::string& filename);
        
        Mtllib m_mtllib;
        std::unordered_map<std::string, Group> m_groups;
        std::unordered_map<std::string, Object> m_objects;
        std::vector<Object*> m_objectsVector;
        std::vector<cv::Point3f> m_vertices;
        Projection m_projection;

    protected:
        std::string m_filename;
        std::string m_filepath;
        std::string m_currentMaterialName;
        std::vector<std::string> m_currentGroupNames;
        std::string m_currentObjectName;

        void readStatement_mtllib(std::string wholeLine, std::istringstream& iss);
        void readStatement_usemtl(std::string wholeLine, std::istringstream& iss);
        void readStatement_g(std::string wholeLine, std::istringstream& iss);
        void readStatement_o(std::string wholeLine, std::istringstream& iss);
        void readStatement_v(std::string wholeLine, std::istringstream& iss);
        void readStatement_f(std::string wholeLine, std::istringstream& iss);
        void addFaceToObject(Object& obj, Face& face);
    };
    
    typedef std::function<void(std::string,std::istringstream&)> ObjFileStatementReaderFunction;

    std::ostream& operator<<(std::ostream&, const Face&);
    std::ostream& operator<<(std::ostream&, const Group&);
    std::ostream& operator<<(std::ostream&, const Object&);
    std::ostream& operator<<(std::ostream&, const ObjFile&);

} // namespace obj
} // namespace gnssShadowing

