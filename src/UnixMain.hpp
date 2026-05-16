#include <iostream>

extern int Main(int argc, char** argv);

int main(int argc, char** argv)
{
    std::cout << "LINUX entry point\n";
    return Main(argc, argv);
}