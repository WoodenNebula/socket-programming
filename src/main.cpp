#include "Logger/Logger.h"

#include "CommandLine.h"
#include "Sockets.h"

DECLARE_LOG_CATEGORY(Main);

int main(int argc, char** argv)
{
    //Sockets::InitSocketAPI();
    //Sockets::CSocket Socket;
    //Socket.Init(Address, Port);
    //Socket.Bind();
    //Socket.Listen();
    //Socket.Accept(); Socket.Connect();
    //Socket.Send();
    //Socket.Close();

    auto [bIsClient, sockAddress] = SAppCmdLineArgs::ParseArgs(argc, argv);


    if (bIsClient)
    {
        LOG(LogMain, Info, "[Init] - Starting as client with address : {}", sockAddress.ToString());
        //ChatClient client;
        //client.Run(address);
    }
    else
    {
        LOG(LogMain, Info, "[Init] - Starting as server in port : {}", sockAddress.Port);
        //ChatServer server;
        //server.Run((uint16)nPort);
    }

    return 0;
}
