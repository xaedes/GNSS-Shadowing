#include "obj/mtllib.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>

using namespace std;

namespace gnssShadowing {
namespace obj {
    
    Mtllib::Mtllib() {}

    void Mtllib::purge()
    {
        m_materials.clear();
        m_currentMaterialName = ""; 
    }

    void Mtllib::read(const std::string& filename)
    {
        ifstream fileStream(filename);
        string line;
        unordered_map<string,MtllibStatementReaderFunction> readers({
               {"newmtl",bind(&Mtllib::readStatement_newmtl, this, placeholders::_1, placeholders::_2)},
               {"Ka",    bind(&Mtllib::readStatement_Ka,     this, placeholders::_1, placeholders::_2)},
               {"Kd",    bind(&Mtllib::readStatement_Kd,     this, placeholders::_1, placeholders::_2)}
           });
        
        while (getline(fileStream, line))
        {
            istringstream iss(line);
            string lineType;
            iss >> lineType;
            if (lineType.length() == 0) continue; // empty line
            if (lineType[0] == '#') continue; // comment
            if (readers.count(lineType))
            {
                readers[lineType](line, iss);
            } 
            else
            {
                // cout << "Unknown statement '" << lineType << "'" << endl;
            }
        }
    }

    ostream& operator<< (ostream& os, const Mtllib& obj) 
    {
        // for (auto it = obj.m_materials.begin(); it != obj.m_materials.end(); ++it)
        // {
        //     os << "Material " << it->first << endl;
        //     os << "ambient:" << it->second.ambient << endl;
        //     os << "diffuse:" << it->second.diffuse << endl;
        // }
        for (auto item : obj.m_materials)
        {
            os << "Material " << item.first << endl;
            os << "ambient:" << item.second.ambient << endl;
            os << "diffuse:" << item.second.diffuse << endl;
        }
        return os;
    }

    void Mtllib::readStatement_newmtl(std::string wholeLine, std::istringstream& iss)
    {
        iss >> m_currentMaterialName;
        if(m_currentMaterialName.length() > 0)
        {
            m_materials[m_currentMaterialName].name = m_currentMaterialName;
        }
        else
        {
            cerr << "Warning, incomplete command '" << wholeLine << "'" << endl;
        }
    }

    void Mtllib::readStatement_Ka(std::string wholeLine, std::istringstream& iss)
    {
        if(m_currentMaterialName.length() > 0)
        {
            iss >> m_materials[m_currentMaterialName].ambient.x 
                >> m_materials[m_currentMaterialName].ambient.y 
                >> m_materials[m_currentMaterialName].ambient.z;
        }
        else
        {
            cerr << "Warning. No material specified! '" << wholeLine << "'" << endl;

        }
    }

    void Mtllib::readStatement_Kd(std::string wholeLine, std::istringstream& iss)
    {
        if(m_currentMaterialName.length() > 0)
        {
            iss >> m_materials[m_currentMaterialName].diffuse.x 
                >> m_materials[m_currentMaterialName].diffuse.y 
                >> m_materials[m_currentMaterialName].diffuse.z;
        }
        else
        {
            cerr << "Warning. No material specified! '" << wholeLine << "'" << endl;

        }
    }

} // namespace obj
} // namespace gnssShadowing

