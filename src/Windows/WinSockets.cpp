#include "WinSockets.h"
#include "SocketsAPI.h"

#include <WinSock2.h>
#include <ws2tcpip.h>

#include "Logger/Logger.h"
// Initialize Winsock

DECLARE_LOG_CATEGORY(WinSockets);


namespace Sockets
{

bool CSocketAPI::Init()
{
    if (s_IsInitialized)
        return s_IsInitialized;

    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        LOG(LogWinSockets, Error, "WinSocketAPI Startup Failed: {}", result);
        return false;
    }
    LOG(LogWinSockets, Success, "WinSocketAPI Initialized");
    return s_IsInitialized = true;
}

void CSocketAPI::Shutdown()
{
    int err = WSACleanup();
    if (err != 0)
    {
        LOG(LogWinSockets, Fatal, "WinSocketAPI Shutdown Failed {}", err);
        exit(err);
    }
    LOG(LogWinSockets, Success, "WinSocketAPI Shutdown");
    return;
}
}


namespace Sockets
{
CWinSockets::~CWinSockets()
{
    LOG(LogWinSockets, Info, "Destroying socket Object");
}

bool CWinSockets::Init(const SAddress& Address)
{
    if (m_bIsInitialized)
        return true;

    m_Address = Address;
    SNativeAddress hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;


    int iResult = getaddrinfo(
        SAddress::GetAddressAsString(Address.IPv4).c_str(),
        SAddress::GetPortAsString(Address.Port).c_str(),
        &hints,
        &m_NativeSocketAddr
    );

    if (iResult != 0)
    {
        LOG(LogWinSockets, Error, "getaddrinfo failed: {}", iResult);
        return false;
    }

    m_NativeSocket = socket(m_NativeSocketAddr->ai_family, m_NativeSocketAddr->ai_socktype, m_NativeSocketAddr->ai_protocol);

    if (m_NativeSocket == INVALID_SOCKET)
    {
        LOG(LogWinSockets, Error, "Error at socket(): {}", WSAGetLastError());
        freeaddrinfo(m_NativeSocketAddr);
        return false;
    }


    LOG(LogWinSockets, Success, "Initializing socket");
    m_bIsInitialized = true;
    return m_bIsInitialized;
}


/// <summary>
/// Start socket as a server socket to broadcast to a particular address
/// </summary>
void CWinSockets::Bind()
{
    if (!m_bIsInitialized)
    {
        LOG(LogWinSockets, Error, "Socket isn't initialized for binding");
        //return false;
    }

    int iResult = bind(m_NativeSocket, m_NativeSocketAddr->ai_addr, (int)m_NativeSocketAddr->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        LOG(LogWinSockets, Error, "Binding failed: {}", WSAGetLastError());
        Shutdown();
    }

    LOG(LogWinSockets, Info, "Socket bound to address: {}", m_Address.ToString());
    // native socket address not required after binding is done
    freeaddrinfo(m_NativeSocketAddr);
}

void CWinSockets::Listen()
{
    if (!m_bIsInitialized)
    {
        LOG(LogWinSockets, Error, "Socket isn't initialized for listening");
        //return false;
    }

    if (listen(m_NativeSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        LOG(LogWinSockets, Error, "Listening failed: {}", WSAGetLastError());
        Shutdown();
    }

    LOG(LogWinSockets, Info, "Socket Listening at address: {}", m_Address.ToString());
}

void CWinSockets::Shutdown()
{
    if (!m_bIsInitialized)
        return;

    freeaddrinfo(m_NativeSocketAddr);
    closesocket(m_NativeSocket);

    LOG(LogWinSockets, Success, "Shutting down socket");
    m_bIsInitialized = false;
}

}   // namespace Sockets
