#include "CommandLine.h"
#include "Sockets.h"
#include "Logger/Logger.h"

DECLARE_LOG_CATEGORY(CmdLineParser);

void SAppCmdLineArgs::PrintUsageAndExit(int rc)
{
    std::cerr.flush();
    std::cout <<
        R"usage(Usage:
    example_chat client SERVER_ADDR
    example_chat server [--port PORT]
)usage" << std::endl;
    exit(rc);
}

SAppCmdLineArgs SAppCmdLineArgs::ParseArgs(int argc, char** argv)
{
    constexpr std::string_view CLIENT = "client";
    constexpr std::string_view SERVER = "server";
    constexpr std::string_view PORT_ARG = "--port";

    Sockets::SAddress serverAddress;
    serverAddress.Clear();

    bool bIsClient = false, bIsServer = false;
    for (int i = 1; i < argc; ++i)
    {
        // First arguement
        if (!bIsClient && !bIsServer)
        {
            if (!CLIENT.compare(argv[i]))
            {
                bIsClient = true; continue;
            }
            if (!SERVER.compare(argv[i]))
            {
                bIsServer = true; continue;
            }
        }

        // Second Arguement: as Server
        if (bIsServer)
        {
            if (!PORT_ARG.compare(argv[i]))
            {
                ++i;
                // --port without PORT_NUM
                if (i >= argc)
                {
                    PrintUsageAndExit();
                }

                int serverPort = atoi(argv[i]);
                if (!Sockets::SAddress::IsPortInRange(serverPort))
                {
                    LOG(LogCmdLineParser, Fatal, "Address Parsing: Invalid port {}", serverPort);
                }

                serverAddress.Port = serverPort;
                continue;
            }
        }
        // Second Arguement as Client
        if (bIsClient)
        {
            serverAddress = Sockets::SAddress::FromString(argv[i]);

            if (!serverAddress)
            {
                LOG(LogCmdLineParser, Fatal, "Address Parsing: Invalid server address {}", argv[i]);
            }
            if (serverAddress.Port == 0)
            {
                serverAddress.Port = Sockets::SAddress::DefaultServerPort();
            }
            continue;
        }

        PrintUsageAndExit();
    }

    if (bIsClient && !serverAddress)
    {
        LOG(LogCmdLineParser, Warning, "Address Parsing: No address supplied as client, using default address: {}", Sockets::SAddress::DefaultServerAddress());
        return { true, Sockets::SAddress::DefaultServerAddress() };
    }
    else if (bIsServer && serverAddress.Port == 0)
    {
        LOG(LogCmdLineParser, Warning, "Address Parsing: No port supplied as server, using default port: {}", Sockets::SAddress::DefaultServerPort());
        serverAddress.Port = Sockets::SAddress::DefaultServerPort();
        return { false, serverAddress };
    }
    else
    {
        return { bIsClient, serverAddress };
    }
}
