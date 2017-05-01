#pragma once

#include <set>           // for std::set
#include <unordered_set> // for std::unordered_set
#include <iterator>      // for std::inserter
#include <algorithm>     // for std::set_intersection, std::sort

namespace gnssShadowing {
namespace common {

    template <typename T>
    std::set<T> intersect(std::set<T>& first, std::set<T>& second)
    {
        std::set<T> intersection;
        std::set_intersection(first.begin(),first.end(),second.begin(),second.end(),
                         std::inserter(intersection,intersection.begin()));
        /*
        for(auto item:intersection)
        {
            assert(first.count(item));
            assert(second.count(item));
        }
        for(auto item:first)
        {
            if(!intersection.count(item)) assert(!second.count(item));
        }
        for(auto item:second)
        {
            if(!intersection.count(item)) assert(!first.count(item));
        }
        */
        return intersection;
    }

    template <typename T>
    void removeDuplicatesInplace(std::vector<T>& vec)
    {
        //http://stackoverflow.com/a/24477023/798588
        std::unordered_set<T> s;
        for (T i : vec)
            s.insert(i);
        vec.assign(s.begin(), s.end());
        std::sort(vec.begin(), vec.end());
    }

    template <typename T>
    std::vector<T> removeDuplicates(const std::vector<T>& vec)
    {
        std::vector<T> copy(vec.begin(), vec.end());
        removeDuplicatesInplace<T>(copy);
        return copy;
    }

} // namespace common
} // namespace gnssShadowing
