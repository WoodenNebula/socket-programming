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

    SNativeAddress hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
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

    SOCKET ConnectSocket = INVALID_SOCKET;
    ConnectSocket = socket(m_NativeSocketAddr->ai_family, m_NativeSocketAddr->ai_socktype, m_NativeSocketAddr->ai_protocol);

    if (ConnectSocket == INVALID_SOCKET)
    {
        LOG(LogWinSockets, Error, "Error at socket(): {}", WSAGetLastError());
        freeaddrinfo(m_NativeSocketAddr);
        return false;
    }


    LOG(LogWinSockets, Success, "Initializing socket");
    m_bIsInitialized = true;
    return m_bIsInitialized;
}

void CWinSockets::Shutdown()
{
    if (!m_bIsInitialized)
        return;

    freeaddrinfo(m_NativeSocketAddr);

    LOG(LogWinSockets, Success, "Shutting down socket");
    m_bIsInitialized = false;
}

}   // namespace Sockets
