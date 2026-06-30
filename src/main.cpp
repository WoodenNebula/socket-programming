#include "Logger/Logger.h"

#include "Utils/CommandLine.h"
#include "SocketsAPI.h"
#include "Sockets.h"

DECLARE_LOG_CATEGORY(Main);

int main(int argc, char** argv)
{
    Sockets::CSocketAPI::Init();
    // [X] Sockets::CSocket Socket;
    // [X] Socket.Init(Address, Port);
    // [.] Socket.Bind();
    // [.] Socket.Listen();
    // [.] Socket.Accept(); Socket.Connect();
    // [.] Socket.Send();
    // [.] Socket.Close();

    auto [bIsClient, sockAddress] = SAppCmdLineArgs::ParseArgs(argc, argv);



    if (bIsClient)
    {
        LOG(LogMain, Info, "[Init] - Starting as client with address : {}", sockAddress.ToString());
        Sockets::CSocket Socket;
        Socket.Init(sockAddress);
        Socket.Connect();
        do
        {

        } while (true);

        //ChatClient client;
        //client.Run(address);
    }
    else
    {
        LOG(LogMain, Info, "[Init] - Starting as server in port : {}", sockAddress.Port);
        Sockets::CSocket Socket;
        Socket.Init(sockAddress);
        Socket.Bind();
        Socket.Listen();
        Socket.Accept();
        do
        {

        } while (true);
        //ChatServer server;
        //server.Run((uint16)nPort);
    }

    Sockets::CSocketAPI::Shutdown();

    return 0;
}
