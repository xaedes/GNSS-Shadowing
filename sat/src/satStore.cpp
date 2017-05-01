
#include "sat/satStore.h"

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

namespace gnssShadowing {
namespace sat {

    SatStore::SatStore(const string& filename) : m_filename(filename)
    {
        ifstream fileStream(filename);
        vector<string> lines;
        string line;
        while(getline(fileStream, line))
        {
            lines.push_back(line);
            if(lines.size() == 3)
            {
                Sat sat(lines[0],lines[1],lines[2]);
                m_sats.push_back(sat);
                // cout << lines[0] << endl;
                lines.clear();
            }
        }

    }

} //namespace sat
} // namespace gnssShadowing

