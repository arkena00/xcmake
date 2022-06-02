#include <xc/xcmake.hpp>
#include <utility>

#include <xc/mapping.hpp>
#include <xc/utility.hpp>

#include <reproc++/run.hpp>

#include <cassert>
#include <filesystem>
#include <fstream>
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
        , clogger_{ [this](std::string data) { log_cmake(std::move(data)); } }
        , xlogger_{ [this](std::string data) { log_xmake(std::move(data)); } }
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
        else if (has_parameter("G")) generate();
        else if (has_parameter("build")) build();
        else
        {
            log("unknown command");
        }
    }

    void xcmake::run(std::vector<std::string>& reproc_args, const xcmake::output_callback& out) const
    {
        reproc::options options;
        options.working_directory = working_directory_.c_str();

        if (verbose_)
        {
            std::string command;
            for (const auto& arg : reproc_args) command.append(arg + " ");
            std::string data;
            reproc::sink::string sink{ data };
            reproc::run(reproc_args, options, sink, sink);
            std::cout << "[xc:run] " << command << "\n" << data;
            return;
        }
        else
        {
            xc::sink sink{ out };
            reproc::run(reproc_args, options, sink, sink);
        }
    }

    void xcmake::run(std::string_view program, const std::vector<std::string>& args, const xcmake::output_callback& out) const
    {
        std::vector<std::string> reproc_args{ args.size() + 1 };
        reproc_args[0] = program;
        std::copy(args.begin(), args.end(), reproc_args.begin() + 1);
        run(reproc_args, out);
    }

    void xcmake::run(std::string_view program, const char* args, const xcmake::output_callback& out) const
    {
        std::vector<std::string> reproc_args{ 2 };
        reproc_args[0] = program;
        reproc_args[1] = args;
        run(reproc_args, out);
    }


    void xcmake::xmake_configure() const
    {
        /*
        std::vector<std::string> values;
        values.emplace_back("config");
        std::string out;
        working_directory_ = std::string("D:\\tmp\\xtest");
        run("xmake", { "config", "-v", "--confirm=n" }, [&out](std::string data) { out += data; });
        out = out.substr(14);

        for (auto line : std::views::split(out, "\n"))
        {
            std::string param = "--" + std::string{ line.begin(), line.end() };
            xc::str_replace(param, " ", "");
            if (!param.empty()) values.emplace_back(param);
        }

        run("xmake", values, xlogger_);*/
    }

    const xc::parameter& xcmake::parameter(const std::string& name) const
    {
        auto parameter_it = std::find_if(parameters_.begin(), parameters_.end(), [&name](const auto& parameter) { return parameter.name == name; });
        assert(parameter_it != parameters_.end() && "parameter not found");
        return *parameter_it;
    }

    std::string xcmake::parameter_value(const std::string& name, const std::string& default_value) const
    {
        auto parameter_it = std::find_if(parameters_.begin(), parameters_.end(), [&name](const auto& parameter) { return parameter.name == name; });
        if (parameter_it != parameters_.end()) return parameter_it->value;
        else return default_value;
    }

    bool xcmake::has_parameter(const std::string& name, xc::parameter_type type) const
    {
        return parameters_.end() != std::find_if(parameters_.begin(), parameters_.end(), [&name, type](const auto& parameter) {
                   return parameter.name == name && parameter.type == type;
               });
    }
    std::string xcmake::cmake_version() const
    {
        std::string out;
        run("cmake", "--version", [&out](std::string data) { out = std::move(data); });

        using namespace std::views;
        auto view = all(out) | drop(14) | take_while([](char c) { return c != '\n'; }) | common;

        return std::string{ view.begin(), view.end() };
    }
    std::string xcmake::xmake_version() const
    {
        std::string out = *new std::string;
        run("xmake", "--version", [&out](std::string data) { out += data; });

        using namespace std::views;
        auto view = all(out) | drop_while([](char c) { return c != 'v'; }) | take_while([](char c) { return c != ','; }) | common;

        return std::string{ view.begin(), view.end() };
    }

    std::string xcmake::color(const std::string& data, const std::string& color) const
    {
        if (!colorize_console_) return data;
        return "\033[" + color + "m" + data + "\033[0m";
    }

    void xcmake::build()
    {
        colorize_console_ = true;
        auto build_path = parameter("build").value;
        working_directory_ = "D:\\tmp\\xtest";

        //if (build_path != prev build_path) generate();

        std::string mode;
        if (build_path.find("debug") != std::string::npos) mode = "debug";
        else if (build_path.find("releasedbg") != std::string::npos) mode = "releasedbg";
        else mode = "release";

        run("xmake", { "config", "--import=./" + mode + ".txt" }, xlogger_);

        auto target_name = parameter_value("target", "all");
        auto target = target_name;
        if (target == "clean")
        {
            run("xmake", "clean", xlogger_);
            return;
        }
        else if (target_name == "all") target = "--all";

        log("initialize build | target {} | mode {}", color(target_name, "92"), color(mode, "92"));

        auto time = std::chrono::system_clock::now();
        run("xmake", { "build", target }, [this, &time](std::string data) {
            if (data.find("100%") != std::string::npos)
            {
                log("build success in {}", xc::to_string(std::chrono::system_clock::now() - time));
            }
            else log_xmake(data);
        });

        colorize_console_ = false;
    }

    void xcmake::generate()
    {
        std::string source_directory = ".";
        if (has_parameter("S")) source_directory = parameter("S").value;
        else source_directory = parameters_.back().value;
        log("source directory: {}", source_directory);
        working_directory_ = source_directory;

        auto mode = xc::xmake_value[parameter_value("CMAKE_BUILD_TYPE", "Release")];

        log("input configuration | mode {}", mode);
        std::vector<std::string> xmake_config;
        xmake_config.emplace_back("config");
        xmake_config.emplace_back("-m");
        xmake_config.emplace_back(mode);
        xmake_config.emplace_back("--export=./" + mode + ".txt");
        for (const auto& parameter : parameters_)
        {
            if (parameter.type == xc::parameter_type::user)
            {
                log("-- {}", parameter.name + " : " + parameter.value);
                xmake_config.emplace_back("--" + parameter.name + "=" + parameter.value);
            }
            else if (parameter.type == xc::parameter_type::user_cmake)
            {
                // if (has_parameter("CMAKE_BUILD_TYPE")) xmake_config.emplace_back("-m " + xc::xmake_value[parameter.value]);
            }
        }
        log("configure");
        run("xmake", xmake_config, xlogger_);

        log("generate cmakelists");
        run("xmake", { "project", "-k", "cmake", "-y" }, xlogger_);

        auto clog = [this](std::string data) {
            if (data == "-- Generating done\n") log("cmake generation succeed");
        };
        //log("forward cmake initialization");
        working_directory_ = "D:\\tmp\\xtest";
        run("cmake", args_, clog);
    }
} // xc