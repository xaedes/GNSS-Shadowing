#include "obj/objFile.h"
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>

using namespace std;

namespace gnssShadowing {
namespace obj {

    ObjFile::ObjFile(const std::string& filename)
    {
        purge();
        read(filename);
    }

    void ObjFile::purge()
    {
        m_projection.valid = false;
        m_mtllib.purge();
        m_currentMaterialName = "";
        m_groups.clear();
        m_groups["default"].name = "default";
        m_currentGroupNames.clear();
        m_currentGroupNames.push_back("default");
        m_vertices.clear();
        m_objectsVector.clear();
    }
    inline bool fileExists (const std::string& name) 
    {
        // http://stackoverflow.com/a/12774387/798588
        struct stat buffer;   
        return (stat (name.c_str(), &buffer) == 0); 
    }
    void ObjFile::read(const std::string& filename)
    {
        purge();
        m_filename = filename;
        assert(fileExists(m_filename));

        m_filepath = m_filename.substr(0, m_filename.find_last_of("\\/")+1);
        ifstream fileStream(m_filename);
        string line;
        unordered_map<string,ObjFileStatementReaderFunction> readers({
               {"mtllib",bind(&ObjFile::readStatement_mtllib, this, placeholders::_1, placeholders::_2)},
               {"usemtl",bind(&ObjFile::readStatement_usemtl, this, placeholders::_1, placeholders::_2)},
               {"g",     bind(&ObjFile::readStatement_g,      this, placeholders::_1, placeholders::_2)},
               {"o",     bind(&ObjFile::readStatement_o,      this, placeholders::_1, placeholders::_2)},
               {"v",     bind(&ObjFile::readStatement_v,      this, placeholders::_1, placeholders::_2)},
               {"f",     bind(&ObjFile::readStatement_f,      this, placeholders::_1, placeholders::_2)}
           });

        while (getline(fileStream, line))
        {
            istringstream iss(line);
            string lineType;
            iss >> lineType;
            if (lineType.length() == 0) continue; // empty line

            if (line[0] == '#' && line.find("Projection") != std::string::npos)
            {
                // projection information are multiline, so the ObjFileStatementReaderFunction wont work here
                fileStream >> m_projection;
                continue;
            };
            if (lineType[0] == '#') continue; // comment
            if (readers.count(lineType))
            {
                readers[lineType](line, iss);
            } 
            else
            {
                cerr << "Unknown statement '" << lineType << "'" << endl;
            }
        }
    }

    istream& operator>> (istream &in, Projection &projection)
    {
        string coordOrigin;
        string northDirection;
        assert(getline(in, coordOrigin));
        assert(getline(in, northDirection));
        istringstream issCoordOrigin(coordOrigin);
        istringstream issNorthDirection(northDirection);
        string uselessToken;
        string sLat, sLon, sEle;
        issCoordOrigin >> uselessToken; // "#"
        issCoordOrigin >> uselessToken; // "Coordinate"
        issCoordOrigin >> uselessToken; // "origin"
        issCoordOrigin >> uselessToken; // "(0,0,0):"
        issCoordOrigin >> uselessToken; // "lat"
        issCoordOrigin >> sLat;         // latitude
        issCoordOrigin >> uselessToken; // "lon"
        issCoordOrigin >> sLon;         // longitude
        issCoordOrigin >> uselessToken; // "ele"
        issCoordOrigin >> sEle;         // elevation
        projection.latitude = stof(sLat);
        projection.longitude = stof(sLon);
        projection.elevation = stof(sEle);
        char uselessChar;
        string sNorthX, sNorthY, sNorthZ;
        issNorthDirection >> uselessToken; // "#"
        issNorthDirection >> uselessToken; // "North"
        issNorthDirection >> uselessToken; // "direction:"
        issNorthDirection >> uselessChar; // '('
        issNorthDirection >> sNorthX;
        issNorthDirection >> sNorthY;
        issNorthDirection >> sNorthZ;
        projection.northDirection.x = stof(sNorthX);
        projection.northDirection.y = stof(sNorthY);
        projection.northDirection.z = stof(sNorthZ);
        projection.valid = true;
        return in;
    }


    void ObjFile::readStatement_mtllib(string wholeLine, istringstream& iss)
    {
        string filename;
        iss >> filename;
        if (fileExists(filename))
        {
            m_mtllib.read(filename);
        }
        else if (fileExists(m_filepath + filename))
        {
            m_mtllib.read(m_filepath + filename);
        }
        else
        {
            cerr << "Material library with filename '" << filename << "' not found." << endl;
        }
    }

    void ObjFile::readStatement_usemtl(string wholeLine, istringstream& iss)
    {
        string matName;
        iss >> matName;
        if (m_mtllib.m_materials.count(matName))
        {
            m_currentMaterialName = matName;
        }
        else
        {
            cerr << "Material with name '" << matName << "' not defined." << endl;
        }
    }

    istream& operator>> (istream &in, vector<string> &vec)
    {
        string item;
        while(in >> item)
        {
            vec.push_back(item);
        }
        return in;
    }

    void ObjFile::readStatement_g(string wholeLine, istringstream& iss)
    {
        vector<string> groupNames;
        iss >> groupNames;
        if(groupNames.size() > 0)
        {
            for(string groupName : groupNames)
            {
                m_groups[groupName].name = groupName;
                m_groups[groupName].objFile = this;
            }
            m_currentGroupNames = groupNames;
        }
        else
        {
            cerr << "Warning, incomplete command '" << wholeLine << "'" << endl;
        }
    }

    void ObjFile::readStatement_o(string wholeLine, istringstream& iss)
    {
        string objectName;
        iss >> objectName;
        if(objectName.length() > 0)
        {
            m_currentObjectName = objectName;
            m_objects[m_currentObjectName].name = m_currentObjectName;
            m_objects[m_currentObjectName].objFile = this;

            for (auto groupName : m_currentGroupNames)
            {
                m_groups[groupName].objects.push_back(&m_objects[m_currentObjectName]);
            }

            m_objectsVector.push_back(&m_objects[m_currentObjectName]);
        }
        else
        {
            cerr << "Warning, incomplete command '" << wholeLine << "'" << endl;
        }
    }

    void ObjFile::readStatement_v(string wholeLine, istringstream& iss)
    {
        cv::Point3f vertex;
        iss >> vertex.x >> vertex.y >> vertex.z;

        // TODO: use projection info north direction
        // for northDirection = (0,0,-1):
        float east  = vertex.x;
        float up    = vertex.y;
        float south = vertex.z;

        // we want to project into south east up
        vertex.x = south;
        vertex.y = east;
        vertex.z = up;

        m_vertices.push_back(vertex);
    }
    void ObjFile::addFaceToObject(Object& obj, Face& face)
    {
        for (auto index : face.vertexIndices)
        {
            obj.vertexIndices.insert(index);
        }
        obj.faces.push_back(face);
        obj.facesByMaterial[face.materialName].push_back(face);
    }

    void ObjFile::readStatement_f(string wholeLine, istringstream& iss)
    {
        int index;
        Face face;
        face.materialName = m_currentMaterialName;
        face.objFile = this;
        while (iss >> index)
        {
            // indices in obj files are 1-based, we store them 0-based
            face.vertexIndices.push_back(index-1);
        }
        for (auto groupName : m_currentGroupNames)
        {
            addFaceToObject(m_groups[groupName], face);
        }
        if (m_currentObjectName.length() > 0)
        {
            addFaceToObject(m_objects[m_currentObjectName], face);
        }
    }

    ostream& operator<<(ostream& os, const Face& face)
    {
        for (int index : face.vertexIndices)
        {
            os << index << " ";
        }
        os << endl;
        return os;
    }
    ostream& operator<<(ostream& os, const Group& grp)
    {
        os << "group " << grp.name << endl;
        // for (auto item : grp.facesByMaterial)
        // {
        //     os << "material " << item.first << endl;
        //     for (Face face : item.second)
        //     {
        //         os << face;
        //     }
        // }
        return os;
    }
    ostream& operator<<(ostream& os, const Object& obj)
    {
        os << "object " << obj.name << endl;
        // for (auto item : obj.facesByMaterial)
        // {
        //     os << "material " << item.first << endl;
        //     for (Face face : item.second)
        //     {
        //         os << face;
        //     }
        // }
        return os;
    }
    ostream& operator<<(ostream& os, const ObjFile& objFile)
    {
        os << objFile.m_mtllib << endl;
        for (auto item : objFile.m_groups)
        {
            os << item.second;
        }
        for (auto item : objFile.m_objects)
        {
            os << item.second;
        }
        return os;
    }

} // namespace obj
} // namespace gnssShadowing

