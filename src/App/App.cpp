#include "App.h"

#include "Sockets.h"
#include "SocketsAPI.h"

#include "Logger/Logger.h"

#include <iostream>
#include <format>
#include <thread>

DECLARE_LOG_CATEGORY(Application);

CApp::CApp() : m_RoomState(ERoomState::None), m_Username("")
{
    bool bAPIInit = Sockets::CSocketAPI::Init();
    if (!bAPIInit)
        LOG(LogApplication, Fatal, "Sockets API initialization failed!");
}

CApp::~CApp()
{
    Sockets::CSocketAPI::Shutdown();
}

bool CApp::Init(ERoomState RoomState, std::string_view Username)
{
    m_RoomState = RoomState;
    m_Username = Username;

    if (m_RoomState == ERoomState::None)
        m_RoomState = PromptHostOrJoin();
    if (m_Username.empty())
        m_Username = PromptUsername();

    return true;
}

void CApp::Host()
{
    Sockets::SAddress HostAddress;
    HostAddress.Port = GetDefaultPort();
    PrintRoomAddress(HostAddress);
    LOG(LogApplication, Info, "Started app as Host on address {}", HostAddress.ToString());

    Sockets::CSocket Host;

    Host.Init(HostAddress);
    Host.Bind();
    Host.Listen();
    auto Client = Host.Accept();
    Print("=====");
    HandleConnection(std::move(Client));
    //Host.Close();
}

void CApp::HandleConnection(std::unique_ptr<Sockets::CSocket> Connection)
{
    std::atomic_bool bIsConnected = true;

    using EResponse = Sockets::ERecieveResponse;
    std::thread receiver([&] {
        Sockets::SSocketPayload payload;

        EResponse Response = EResponse::Ok;
        while (true)
        {
            Response = Connection->Receive(payload);

            if (Response != EResponse::Ok)
            {
                Print("{}<{}> Remote shutdown!{}", COLOR_YELLOW, m_Username, COLOR_RESET);
                break; // exit from thread by reaching the end of the thread's lambda
            }

            Print("<{}> -> {}", m_Username, payload.BufferAsString());
        }
        bIsConnected = false;
        Print("<{}> Receiver finished", m_Username);
        });

    while (true)
    {
        std::string msg;
        std::getline(std::cin >> std::ws, msg);
        if (!bIsConnected)
            break;
        if (msg == ":q")
        {
            Print("{}<{}> Self shutdown!{}", COLOR_YELLOW, m_Username, COLOR_RESET);
            Connection->Shutdown();
            break;
        }
        std::string color = m_RoomState == ERoomState::Host ? COLOR_GREEN : COLOR_CYAN;
        Sockets::SSocketPayload payload(std::format("{}[{}] : {}{}", color, m_Username, msg, COLOR_RESET));

        if (!Connection->Send(payload))
            break; // sending failed, probably client disconnected
    }

    receiver.join();
    Print("<{}> Receiver joined", m_Username);
    Connection->Close();
    Print("<{}> Connection Closed", m_Username);
}

void CApp::Join()
{
    Sockets::SAddress HostAddress = PromptRoomAddress();
    LOG(LogApplication, Info, "Started app as Client to Join address {}", HostAddress.ToString());
    std::unique_ptr<Sockets::CSocket> Host = std::make_unique<Sockets::CSocket>();

    Host->Init(HostAddress);
    Host->Connect();

    Print("=====");
    HandleConnection(std::move(Host));
}

Sockets::SAddress CApp::PromptRoomAddress()
{
    std::string address;
    Sockets::SAddress HostAddress;
    while (true)
    {
        Print("Enter room address to join('d' for 127.0.0.1): example -> 192.168.1.1");
        std::cin >> address;
        if (address.at(0) == 'd')
            HostAddress = Sockets::SAddress::FromString(Sockets::SAddress::DefaultServerAddress());
        else
        {
            HostAddress = Sockets::SAddress::FromString(std::format("{}:{}", address, GetDefaultPort()));
        }
        if (HostAddress.IsValid()) return HostAddress;
        else
        {
            Print("{}====\n{}{} is not a valid address", COLOR_YELLOW, COLOR_RED, HostAddress.ToString());
            Print("{}Please enter a valid address\n===={}", COLOR_YELLOW, COLOR_RESET);
        }
    }

    return {};
}

void CApp::PrintRoomAddress(const Sockets::SAddress SelfAddress) const
{
    Print("Room address: {}", SelfAddress.ToString());
}

void CApp::Run()
{
    switch (m_RoomState)
    {
    case ERoomState::Host:
        Host();
        break;
    case ERoomState::Client:
        Join();
        break;
    case ERoomState::None:
    default:
        LOG(LogApplication, Fatal, "Unreachable!");
        break;
    }
}

ERoomState CApp::PromptHostOrJoin()
{
    while (true)
    {
        Print("Host(h) or Join(j) room?");
        char c = 0;
        std::cin >> c;

        if (c == 'h')
            return ERoomState::Host;
        else if (c == 'j')
            return ERoomState::Client;
        else
            Print("{}====\nPlease enter a valid option\n===={}", COLOR_YELLOW, COLOR_RESET);
    }

    return ERoomState::None;
}

std::string CApp::PromptUsername()
{
    std::string username;
    Print("Enter your username: ");
    std::cin >> username;
    return username;
}