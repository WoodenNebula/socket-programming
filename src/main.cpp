#include "Logger/Logger.h"

#include "Utils/CommandLine.h"
#include "SocketsAPI.h"
#include "Sockets.h"

#include <chrono>
#include <thread>

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

        std::string str = "HELLOW EVERYNEON!";
        Sockets::SSocketPayload Payload(str);

        Socket.Send(Payload);

        using namespace std::chrono_literals;

        int i = 1;

        while (1)
        {
            if (i < 5)
            {
                std::this_thread::sleep_for(2s);

                str = std::format("{} Hello again!", i);

                Payload = Sockets::SSocketPayload(str);
                Socket.Send(Payload);
                i++;
            }
            else break;
        }


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
        Sockets::SSocketPayload Payload;
        Socket.Receive(Payload);
        //ChatServer server;
        //server.Run((uint16)nPort);
    }

    Sockets::CSocketAPI::Shutdown();

    return 0;
}
