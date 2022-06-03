#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include <xc/xcmake.hpp>

int main(int argc, char* argv[])
{
    std::vector<std::string> args(argv + 1, argv + argc);

    try
    {
        xc::xcmake xcmake{ std::string{ argv[0] }, std::move(args) };

        xcmake.process();
    }
    catch (const std::exception& e)
    {
        std::cout << "xc exception: " << e.what();
    }

    return 0;
}