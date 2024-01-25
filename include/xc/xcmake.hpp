#ifndef XC_XCMAKE_HPP_XCMAKE
#define XC_XCMAKE_HPP_XCMAKE

#include <xc/parameter.hpp>
#include <xc/utility.hpp>


#include <fmt/core.h>
#include <functional>
#include <iostream>
#include <ranges>

#include <string>
#include <vector>

namespace xc
{
    class xcmake
    {
        using output_callback = std::function<void(std::string)>;

    public:
        xcmake(std::string program, std::vector<std::string> args);
        xcmake(const xcmake&) = delete;
        xcmake& operator=(const xcmake&) = delete;

        void process();

        void build();
        void generate();

        // run
        void run(
            std::vector<std::string>& reproc_args,
            const xcmake::output_callback& = [](std::string data) {}) const;
        void run(
            std::string_view program,
            const std::vector<std::string>& args,
            const xcmake::output_callback& = [](std::string data) {}) const;
        void run(
            std::string_view program,
            const char* args,
            const xcmake::output_callback& = [](std::string data) {}) const;

        template<typename... Args>
        void error(std::string_view message, Args&&...) const;
        template<typename... Args>
        void log(std::string_view message, Args&&...) const;
        void log_cmake(std::string message) const;
        void log_xmake(std::string message, bool single_line = false) const;

        void xmake_configure() const;

        [[nodiscard]] const xc::parameter& parameter(const std::string& name) const;
        [[nodiscard]] std::string parameter_value(const std::string& name, const std::string& default_value = "") const;
        [[nodiscard]] bool has_parameter(const std::string& name, xc::parameter_type t = xc::parameter_type::cmake) const;

        [[nodiscard]] std::string cmake_version() const;
        [[nodiscard]] std::string xmake_version() const;

        [[nodiscard]] std::string color(const std::string& data, const std::string& color) const;

    private:
        std::string program_;
        std::vector<std::string> args_;
        std::vector<xc::parameter> parameters_;

        std::string working_directory_;
        bool colorize_console_;
        bool verbose_;

        output_callback clogger_;
        output_callback xlogger_;

        int errors_count_;
        int warnings_count_;
    };
} // xc

namespace xc
{
    template<typename... Args>
    void xcmake::error(std::string_view message, Args&&... args) const
    {
        std::cout << "[xc] [xc:" + color("error", "101") + "] " << fmt::vformat(message, fmt::make_format_args(args...)) << std::endl;
    }

    template<typename... Args>
    void xcmake::log(std::string_view message, Args&&... args) const
    {
        std::cout << color("[xc]", "34") << " " << fmt::vformat(message, fmt::make_format_args(args...)) << std::endl;
    }
} // xc

#endif // XC_XCMAKE_HPP_XCMAKE