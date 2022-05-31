#ifndef INCLUDE_XC_PARAMETER_HPP_XCMAKE
#define INCLUDE_XC_PARAMETER_HPP_XCMAKE

#include <string>

namespace xc
{
    enum class parameter_type
    {
        cmake,
        user,
        user_cmake // starting with CMAKE_
    };

    struct parameter
    {
        std::string name;
        std::string value;
        parameter_type type{ parameter_type::cmake };
    };
} // xc

#endif // INCLUDE_XC_PARAMETER_HPP_XCMAKE