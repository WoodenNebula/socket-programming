#pragma once
#include "Sockets.h"

struct SAppCmdLineArgs
{
    bool bIsClient;
    Sockets::SAddress SockAddress;

    static void PrintUsageAndExit(int rc = 1);
    static SAppCmdLineArgs ParseArgs(int argc, char** argv);
};
