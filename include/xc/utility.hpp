#ifndef INCLUDE_XC_UTILITY_HPP_XCMAKE
#define INCLUDE_XC_UTILITY_HPP_XCMAKE


#include <xc/parameter.hpp>

#include <chrono>
#include <functional>
#include <ranges>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace xc
{
    inline void str_replace(std::string& str, const std::string& from, const std::string& to)
    {
        if (from.empty()) return;
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos)
        {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
    }

    template<class Duration>
    inline std::string to_string(const Duration& duration)
    {
        using namespace std::chrono;
        auto h = duration_cast<hours>(duration).count();
        auto m = duration_cast<minutes>(duration).count();
        auto s = duration_cast<seconds>(duration).count();

        std::stringstream ss;
        ss << std::setfill('0') << std::setw(2) << h << ":" <<std::setw(2) << m << ":" << std::setw(2) << s;
        return ss.str();
    }

    inline std::string clean_colors(const std::string& input)
    {
        std::string output;
        bool skip = false;
        for (char c : input)
        {
            if (c == '\033')
            {
                skip = true;
                continue;
            }
            else if (skip)
            {
                if (c == 'm') skip = false;
            }
            else output += c;
        }
        return output;
    }

    inline std::pair<std::string, std::string> parse_user_parameter(const std::string& data)
    {
        auto offset = data.find('=');
        if (offset == std::string::npos || offset + 1 >= data.size()) throw std::logic_error{ "parse_user_parameter " + data };

        return { data.substr(0, offset), data.substr(offset + 1) };
    }

    inline std::vector<xc::parameter> cmake_parse_args(std::vector<std::string> args)
    {
        std::vector<xc::parameter> parameters;

        for (int i = 0; i < args.size(); ++i)
        {
            auto& arg = args[i];
            xc::parameter parameter;

            if (!arg.empty() && arg[0] == '-')
            {
                if (arg.size() > 1)
                {
                    if (arg[1] == '-')
                    {
                        parameter.name = arg.substr(2);
                        if (i + 1 >= args.size())
                        {
                            parameters.emplace_back(std::move(parameter));
                            break;
                        }

                        ++i;
                        parameter.value = args[i];
                        parameters.emplace_back(std::move(parameter));
                        continue;
                    }
                    else if (arg[1] > 'A' && arg[1] < 'Z')
                    {
                        if (arg[1] == 'D') parameter.type = xc::parameter_type::user;

                        if (arg.size() == 2)
                        {
                            ++i;
                            if (parameter.type == xc::parameter_type::user)
                            {
                                auto [user_parameter, user_value] = parse_user_parameter(args[i]);
                                if (user_parameter.starts_with("CMAKE_")) parameter.type = xc::parameter_type::user_cmake;
                                parameter.name = user_parameter;
                                parameter.value = user_value;
                            }
                            else
                            {
                                parameter.name = arg[1];
                                parameter.value = args[i];
                            }
                            parameters.emplace_back(std::move(parameter));
                            continue;
                        }
                        else
                        {
                            if (parameter.type == xc::parameter_type::user)
                            {
                                auto [user_parameter, user_value] = parse_user_parameter(arg.substr(2));
                                if (user_parameter.starts_with("CMAKE_")) parameter.type = xc::parameter_type::user_cmake;
                                parameter.name = user_parameter;
                                parameter.value = user_value;
                            }
                            else
                            {
                                parameter.name = arg[1];
                                parameter.value = arg.substr(2);
                            }
                            parameters.emplace_back(std::move(parameter));
                        }
                    }
                    else
                    {
                        parameter.name = arg.substr(1);
                        parameters.emplace_back(std::move(parameter));
                    }
                }
            }
            else
            {
                parameter.value = arg;
                parameters.emplace_back(std::move(parameter));
            }
        }

        return parameters;
    }
} // xc

#endif // INCLUDE_XC_UTILITY_HPP_XCMAKE