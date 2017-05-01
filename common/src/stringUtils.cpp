
#include "common/stringUtils.h"

#include <sstream>
#include <string>
#include <vector>

using namespace std;

namespace gnssShadowing {
namespace common {


    std::vector<std::string> split(std::string str, char delimiter)
    {
        istringstream ss(str);
        string token;
        vector<string> result;
        while (std::getline(ss, token, delimiter))
        {
            result.push_back(token);
        }
        return result;
    }

} // namespace common
} // namespace gnssShadowing

