#include <iostream>
#include <vector>
#include <string>

#include <windows.h>
#include <shellapi.h>

static std::string WideToUTF8(const wchar_t* wide)
{
    if (!wide)
        return {};

    int sizeNeeded = WideCharToMultiByte(
        CP_UTF8,
        0,
        wide,
        -1,
        nullptr,
        0,
        nullptr,
        nullptr
    );

    std::string result(sizeNeeded - 1, '\0');

    WideCharToMultiByte(
        CP_UTF8,
        0,
        wide,
        -1,
        result.data(),
        sizeNeeded,
        nullptr,
        nullptr
    );

    return result;
}

extern int Main(int argc, char** argv);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
    std::cout << "WINDOWS entry point\n";
    int argc = 0;
    LPWSTR* wideArgv = CommandLineToArgvW(GetCommandLineW(), &argc);

    std::vector<std::string> utf8Args;
    std::vector<char*> argv;

    utf8Args.reserve(argc);
    argv.reserve(argc);

    for (int i = 0; i < argc; ++i)
    {
        utf8Args.push_back(WideToUTF8(wideArgv[i]));
    }

    for (auto& str : utf8Args)
    {
        argv.push_back(str.data());
    }

    int result = Main(argc, argv.data());

    LocalFree(wideArgv);

    return result;
}
