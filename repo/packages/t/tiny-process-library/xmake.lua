package("tiny-process-library")

    set_homepage("https://gitlab.com/eidheim/tiny-process-library")
    set_description("A small platform independent library making it simple to create and stop new processes in C++, as well as writing to stdin and reading from stdout and stderr of a new process")

    set_urls("https://gitlab.com/eidheim/tiny-process-library.git")
    add_versions("v2.0.4", "93edd94959a2cc8aec1ea4ec5df82835b2b05bac")

    on_install(function (package)
        import("package.tools.cmake").install(package, configs)
    end)