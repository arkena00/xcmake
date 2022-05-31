#ifndef INCLUDE_XC_MAPPING_HPP_XCMAKE
#define INCLUDE_XC_MAPPING_HPP_XCMAKE

#include <unordered_map>

namespace xc
{
    static std::unordered_map<std::string, std::string> xmake_value{
        { "Debug", "debug" },
        { "Release", "release" },
        { "RelWithDebInfo", "releasedbg" },
        { "MinSizeRel", "minsizerel" },
    };
} // xcmake

#endif // INCLUDE_XC_MAPPING_HPP_XCMAKE