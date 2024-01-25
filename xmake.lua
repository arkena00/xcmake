add_rules("mode.debug", "mode.release")
set_languages("cxx20")

add_repositories("local repo")
add_requires("fmt", "tiny-process-library")

target("xc")
    add_files("source/**.cpp")
    add_includedirs("include")
    add_packages("fmt", "tiny-process-library")