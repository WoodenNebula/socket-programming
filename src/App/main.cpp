#include "Logger/Logger.h"

#include "Utils/CommandLine.h"
#include "SocketsAPI.h"
#include "Sockets.h"

#include "App.h"

#include <chrono>
#include <thread>

DECLARE_LOG_CATEGORY(Main);

int main(int argc, char** argv)
{
    auto [bIsClient, sockAddress] = SAppCmdLineArgs::ParseArgs(argc, argv);

    ERoomState State = bIsClient ? ERoomState::Client : ERoomState::Host;
    std::string Username = bIsClient ? "Client" : "Host";

    CApp app;
    app.Init(State, Username);
    app.Run();

    return 0;
}
