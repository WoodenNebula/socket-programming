#include "WinSockets.h"
#include "SocketsAPI.h"

#include <WinSock2.h>
#include <ws2tcpip.h>

#include "Logger/Logger.h"
// Initialize Winsock

DECLARE_LOG_CATEGORY(WinSockets);


namespace Sockets
{
#define ENSURE_INITIALIZED(condition, context) \
    do { \
        if (!condition) \
        { \
            LOG(LogWinSockets, Error, "Socket isn't initialized for {}", context); \
            exit(1); \
        }\
    } while(0)


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
    ENSURE_INITIALIZED(m_bIsInitialized, "Binding");

    int iResult = bind(m_NativeSocket, m_NativeSocketAddr->ai_addr, (int)m_NativeSocketAddr->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        LOG(LogWinSockets, Error, "Binding failed: {}", WSAGetLastError());
        Close();
        return;
    }

    LOG(LogWinSockets, Info, "Socket bound to address: {}", m_Address.ToString());
    // native socket address not required after binding is done
    freeaddrinfo(m_NativeSocketAddr);
}

void CWinSockets::Listen()
{
    ENSURE_INITIALIZED(m_bIsInitialized, "Listening");

    if (listen(m_NativeSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        LOG(LogWinSockets, Error, "Listening failed: {}", WSAGetLastError());
        Close();
        return;
    }

    LOG(LogWinSockets, Success, "Socket Listening at address: {}", m_Address.ToString());
}

std::unique_ptr<ISocket> CWinSockets::Accept()
{
    ENSURE_INITIALIZED(m_bIsInitialized, "Connecting");

    SNativeSocket client = accept(m_NativeSocket, nullptr, nullptr);

    if (client == INVALID_SOCKET)
    {
        LOG(LogWinSockets, Error, "Accept failed: {}", WSAGetLastError());
        Close();
        return nullptr;
    }

    auto ClientSocket = std::make_unique<CWinSockets>();

    ClientSocket->m_bIsInitialized = true;
    ClientSocket->m_NativeSocket = client;

    LOG(LogWinSockets, Success, "Socket accepted at address: {}", m_Address.ToString());

    return ClientSocket;
}

void CWinSockets::Connect()
{
    ENSURE_INITIALIZED(m_bIsInitialized, "Connecting");

    if (connect(m_NativeSocket, m_NativeSocketAddr->ai_addr, (int)m_NativeSocketAddr->ai_addrlen) == SOCKET_ERROR)
    {
        LOG(LogWinSockets, Error, "Connecting failed: {}", WSAGetLastError());

        // Should really try the next address returned by getaddrinfo
        // if the connect call failed
        // But for now, we just free the resources
        // returned by getaddrinfo i.e. m_NativeSocketAddr
        Close();
        return;
    }

    if (m_NativeSocket == INVALID_SOCKET)
    {
        LOG(LogWinSockets, Error, "Unable to connect to server");
        return;
    }
    LOG(LogWinSockets, Success, "Socket Connected to address: {}", m_Address.ToString());
}

bool CWinSockets::Send(const SSocketPayload& Payload)
{
    ENSURE_INITIALIZED(m_bIsInitialized, "Sending");

    if (!Payload.Validate())
        return false;


    int iResult;

    // Send a buffer
    iResult = send(m_NativeSocket, Payload.Buffer.data(), static_cast<int>(Payload.GetLength()), 0);
    if (iResult == SOCKET_ERROR)
    {
        LOG(LogWinSockets, Error, "Sending failed: {}", WSAGetLastError());
        Close();
        return false;
    }

    LOG(LogWinSockets, Success, "Sent {} to address: `{}`", Payload.ToString(), m_Address.ToString());
    return true;
}


ERecieveResponse CWinSockets::Receive(SSocketPayload& Payload)
{
    ENSURE_INITIALIZED(m_bIsInitialized, "Receiving");

    if (!Payload.Validate())
        return ERecieveResponse::Err;

    int iResult = recv(m_NativeSocket, Payload.Buffer.data(), SSocketPayload::MAX_LENGTH, 0);
    if (iResult > 0)
    {
        LOG(LogWinSockets, Success, "Received {} bytes: `{}`", iResult, Payload.ToString());
        return ERecieveResponse::Ok;
    }

    else if (iResult == 0)
    {
        LOG(LogWinSockets, Warning, "Received FIN signal aka to close send");
        return ERecieveResponse::Close;
    }
    else
    {
        LOG(LogWinSockets, Error, "Receiving failed: iRes = {}, err = {}", iResult, WSAGetLastError());
        Close();
        return ERecieveResponse::Err;
    }

    /// Can shutdown after the data is received. Do we use and throw sockets?
}

void CWinSockets::Shutdown()
{
    //ENSURE_INITIALIZED(m_bIsInitialized, "Shutdown");

    shutdown(m_NativeSocket, SD_SEND);
    LOG(LogWinSockets, Success, "Shutting down socket");
}

void CWinSockets::Close()
{
    ENSURE_INITIALIZED(m_bIsInitialized, "Closing");
    freeaddrinfo(m_NativeSocketAddr);

    closesocket(m_NativeSocket);
    m_bIsInitialized = false;
    LOG(LogWinSockets, Success, "Closing socket");
}

}   // namespace Sockets
