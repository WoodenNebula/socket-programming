#include "Sockets.h"

#include "Logger/Logger.h"

#if WINDOWS
#include "Windows/WinSockets.h"
#else
#include "Linux/LinuxSockets.h"
#endif

#include <algorithm>

DECLARE_LOG_CATEGORY(Sockets);

namespace Sockets
{
bool SAddress::IsValid() const
{
    bool bIsValid = true;

    for (const auto& octet : IPv4)
    {
        if (bIsValid && IsOctetValid(octet))
            continue;
        else return false;
    }

    return IsPortInRange(Port);
}

std::string SAddress::ToString() const
{
    std::string FullAddress;
    for (const auto& octet : IPv4)
    {
        FullAddress += std::to_string(octet) + '.';
    }
    FullAddress.pop_back();

    FullAddress += ':' + std::to_string(Port);
    return FullAddress;
}

bool SAddress::IsPortInRange(int Port)
{
    return Port > 0 && Port <= UINT16_MAX;
};

bool SAddress::IsOctetValid(int Octet, char Separator)
{
    return (Octet >= 0 && Octet <= UINT8_MAX) && Separator == '.';
};


SAddress SAddress::FromString(std::string_view Address)
{
    size_t separatorPos = Address.find(":");

    if (separatorPos == std::string_view::npos)
    {
        LOG(LogSockets, Fatal, "Invalid address format: {}", Address);
        return {};
    }

    SAddress sockAddress;
    sockAddress.Clear();

    std::string_view IP = Address.substr(0, separatorPos);
    std::string_view PORT = Address.substr(separatorPos + 1);

    size_t octetStart = 0;
    size_t octetNum = 0;
    while (octetNum < 4)
    {
        auto separatorPos = IP.find(".", octetStart);

        int octet = std::stoi(std::string(IP.substr(octetStart, separatorPos - octetStart)));
        if (!IsOctetValid(octet))
        {
            LOG(LogSockets, Fatal, "Invalid address format: {}", Address);
            return {};
        }

        sockAddress.IPv4[octetNum] = octet;

        octetStart = separatorPos + 1;
        octetNum++;
    }

    int port = std::stoi(std::string(PORT));
    if (!IsPortInRange(port))
    {
        LOG(LogSockets, Fatal, "Address Parsing: Invalid Address: {}", Address);
        return {};
    }

    sockAddress.Port = port;
    return sockAddress;
}
}   // namespace Sockets


namespace Sockets
{

SSocketPayload::SSocketPayload() : Buffer{ 0 }
{

}

SSocketPayload::SSocketPayload(std::string_view StringData) : Buffer{ 0 }
{
    std::copy_n(StringData.data(), std::min(StringData.size(), MAX_LENGTH), Buffer.data());
}

bool SSocketPayload::Validate() const
{
    bool bIsValid = (!Buffer.empty() && GetLength() >= 0 && GetLength() <= SSocketPayload::MAX_LENGTH);
    if (bIsValid)
        LOG(LogSockets, Trace, "Payload of len {} is valid: ", GetLength(), ToString());
    else
        LOG(LogSockets, Error, "Invalid Payload: ", ToString());

    return bIsValid;
}

std::string SSocketPayload::ToString() const
{
    return std::format("Len: {}, data: {}", GetLength(), std::string_view(Buffer.data(), Buffer.size()));
}

std::string SSocketPayload::BufferAsString() const
{
    return std::string(Buffer.data(), Buffer.size());
}
}   // namespace Sockets



namespace Sockets
{

CSocket::CSocket()
{
#if WINDOWS
    m_SockImpl = std::make_unique <CWinSockets>();
#else
    m_SockImpl = std::make_unique <CLinuxSockets>();
#endif

}

CSocket::~CSocket()
{}

bool CSocket::Init(const SAddress& Address)
{
    return m_SockImpl->Init(Address);
}

void CSocket::Close()
{
    m_SockImpl->Close();
}

void CSocket::Shutdown()
{
    m_SockImpl->Shutdown();
}

bool CSocket::Send(const SSocketPayload& Payload)
{
    return m_SockImpl->Send(Payload);
}
ERecieveResponse CSocket::Receive(SSocketPayload& Payload)
{
    return m_SockImpl->Receive(Payload);
}
/* Server Related methods */
void CSocket::Bind()
{
    m_SockImpl->Bind();
}
void CSocket::Listen()
{
    m_SockImpl->Listen();
}
std::unique_ptr<CSocket> CSocket::Accept()
{
    auto clientImpl = m_SockImpl->Accept();
    if (!clientImpl)
        return nullptr;

    auto clientSocket = std::make_unique<CSocket>();
    clientSocket->m_SockImpl = std::move(clientImpl);
    return clientSocket;
}

/* Client Related methods */
void CSocket::Connect()
{
    m_SockImpl->Connect();
}
}
