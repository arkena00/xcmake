#ifndef XC_XCMAKE_HPP_XCMAKE
#define XC_XCMAKE_HPP_XCMAKE

#include <xc/parameter.hpp>

#include <reproc++/reproc.hpp>
#include <reproc++/run.hpp>

#include <format>
#include <iostream>
#include <string>
#include <vector>
#include <ranges>

namespace xc
{
    class xcmake
    {
    public:
        xcmake(std::string program, std::vector<std::string> args);
        xcmake(const xcmake&) = delete;
        xcmake& operator=(const xcmake&) = delete;

        void process();

        std::pair<std::string, std::string> run(std::vector<std::string>& reproc_args) const;
        std::pair<std::string, std::string> run(std::string_view program, const std::vector<std::string>& args) const;
        template<class... Ts>
        std::pair<std::string, std::string> run(std::string_view program, Ts... args) const;

        template<typename... Args>
        void error(std::string_view message, Args&&...);
        template<typename... Args>
        void log(std::string_view message, Args&&...);
        template<typename... Args>
        void log_cmake(std::string_view message, Args&&...);
        template<typename... Args>
        void log_xmake(std::string_view message, Args&&...);


        [[nodiscard]] const xc::parameter& parameter(const std::string& name) const;

        [[nodiscard]] bool has_parameter(const std::string& name, xc::parameter_type t = xc::parameter_type::cmake) const;

        [[nodiscard]] std::string cmake_version() const;
        [[nodiscard]] std::string xmake_version() const;

        std::string color(const std::string& data, const std::string& color);

    private:
        std::string program_;
        std::vector<std::string> args_;
        std::vector<xc::parameter> parameters_;

        std::string working_directory_;
        bool colorize_console_;
        bool verbose_;
    };
} // xc

namespace xc
{
    template<class... Ts>
    std::pair<std::string, std::string> xcmake::run(std::string_view program, Ts... args) const
    {
        std::vector<std::string> reproc_args;
        reproc_args.reserve(sizeof...(Ts) + 1);
        reproc_args.emplace_back(program);
        (reproc_args.emplace_back(args), ...);

        return run(reproc_args);
    }

    template<typename... Args>
    void xcmake::error(std::string_view message, Args&&... args)
    {
        std::cout << "[xc] [xc:" + color("error", "101") + "] " << std::vformat(message, std::make_format_args(args...)) << std::endl;
    }

    template<typename... Args>
    void xcmake::log(std::string_view message, Args&&... args)
    {
        std::cout << "[xc] " << std::vformat(message, std::make_format_args(args...)) << std::endl;;
    }

    template<typename... Args>
    void xcmake::log_cmake(std::string_view message, Args&&... args)
    {
        std::cout << ("[xc:" + color("cmake", "33") + "] ") << std::vformat(message, std::make_format_args(args...)) << std::endl;
    }

    template<typename... Args>
    void xcmake::log_xmake(std::string_view message, Args&&... args)
    {
        std::cout << ("[xc:" + color("xmake", "96") + "] ") << std::vformat(message, std::make_format_args(args...)) << std::endl;
    }
} // xc

#endif // XC_XCMAKE_HPP_XCMAKE