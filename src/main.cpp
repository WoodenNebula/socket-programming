#if WINDOWS
#include "WinMain.hpp"
#else
#include "UnixMain.hpp"
#endif

int Main(int argc, char** argv)
{
    std::cout << "CUSTOM entry point\n";

    return 0;
}