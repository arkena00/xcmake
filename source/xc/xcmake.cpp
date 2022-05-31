#include <xc/xcmake.hpp>

#include <xc/utility.hpp>
#include <xc/mapping.hpp>

#include <reproc++/run.hpp>

#include <cassert>
#include <iostream>
#include <ranges>

namespace xc
{
    xcmake::xcmake(std::string program, std::vector<std::string> args)
        : program_{ std::move(program) }
        , args_{ std::move(args) }
        , parameters_{ cmake_parse_args(args_) }
        , working_directory_{ "." }
        , colorize_console_{ false }
        , verbose_{ true }
    {}

    void xcmake::process()
    {
        if (parameters_.empty())
        {
            log("run");
            run("xmake", "run");
            return;
        }
        else if (parameters_[0].name == "version")
        {
            auto verbose = verbose_;
            verbose_ = false;
            std::cout << "cmake version cmake " << cmake_version() << " | xmake " << xmake_version() << " | xcmake 1.0";
            verbose_ = verbose;
            return;
        }
        else if (has_parameter("G"))
        {
            if (!has_parameter("S")) return error("missing source directory");
            log("working directory: {}", parameter("S").value);
            working_directory_ = parameter("S").value;

            log("input configuration");
            std::string xmake_config;
            for (const auto& parameter : parameters_)
            {
                if (parameter.type == xc::parameter_type::user)
                {
                    log("-- {}", parameter.name + " : " + parameter.value);
                    xmake_config += " --" + parameter.name + "=" + parameter.value;
                }
                else if (parameter.type == xc::parameter_type::user_cmake)
                {
                     if (has_parameter("CMAKE_BUILD_TYPE")) xmake_config += " -m " + xc::xmake_value[parameter.value];
                }
            }
            log_xmake("configure");
            log_xmake(xmake_config);
            run("xmake", "config", "zeta=on", "test=on");

            log_xmake("generate cmakelists");
            run("xmake", "project", "-k", "cmake", "-y");

            log("run cmake");
            auto [out, err] = run("cmake", args_);
            log_cmake(out);
        }
        else if (parameters_[0].name == "build")
        {
            colorize_console_ = true;
            auto target = parameter("target").value;

            log_xmake("build target {}", color(target, "92"));

            if (target == "all") target = "--all";
            run("xmake", "build", target);

            colorize_console_ = false;
            return;
        }
        else
        {
            log("unknown command");
        }
    }

    std::pair<std::string, std::string> xcmake::run(std::vector<std::string>& reproc_args) const
    {
        reproc::options options;
        options.working_directory = working_directory_.c_str();

        std::string out;
        std::string err;
        reproc::sink::string sink_out{ out };
        reproc::sink::string sink_err{ err };

        reproc::run(reproc_args, options, sink_out, sink_err);
        if (verbose_) std::cout << out << "\n" << err;
        return { out, err };
    }

    std::pair<std::string, std::string> xcmake::run(std::string_view program, const std::vector<std::string>& args) const
    {
        std::vector<std::string> reproc_args{ args.size() + 1 };
        reproc_args[0] = program;
        std::copy(args.begin(), args.end(), reproc_args.begin() + 1);

        return run(reproc_args);
    }

    const xc::parameter& xcmake::parameter(const std::string& name) const
    {
        auto parameter_it = std::find_if(parameters_.begin(), parameters_.end(), [&name](const auto& parameter) { return parameter.name == name; });
        assert(parameter_it != parameters_.end() && "parameter not found");
        return *parameter_it;
    }

    bool xcmake::has_parameter(const std::string& name, xc::parameter_type type) const
    {
        return parameters_.end() != std::find_if(parameters_.begin(), parameters_.end(), [&name, type](const auto& parameter) {
                   return parameter.name == name && parameter.type == type;
               });
    }
    std::string xcmake::cmake_version() const
    {
        auto [out, err] = run("cmake", "--version");

        using namespace std::views;
        auto view = all(out) | drop(14) | take_while([](char c) { return c != '\n'; }) | common;

        return std::string{ view.begin(), view.end() };
    }
    std::string xcmake::xmake_version() const
    {
        auto [out, err] = run("xmake", "--version");

        using namespace std::views;
        auto view = all(out) | drop_while([](char c) { return c != 'v'; }) | take_while([](char c) { return c != ','; }) | common;

        return std::string{ view.begin(), view.end() };
    }

    std::string xcmake::color(const std::string& data, const std::string& color)
    {
        if (!colorize_console_) return data;
        return "\033[" + color + "m" + data + "\033[0m";
    }
} // xc