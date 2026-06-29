#include "Sockets.h"

#include "Logger/Logger.h"

#if WINDOWS
#include "Windows/WinSockets.h"
#else
#include "Linux/LinuxSockets.h"
#endif

DECLARE_LOG_CATEGORY(Sockets);

namespace Sockets
{
bool SAddress::IsValid() const
{
    bool bIsValid = true;

    Octet_t last = IPv4.at(0);
    for (size_t i = 1; i < IPv4.size(); i++)
    {
        bIsValid = bIsValid && (last != IPv4.at(i));
        last = IPv4.at(i);
    }
    return bIsValid;
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
CSocket::CSocket()
{
#if WINDOWS
    m_SockImpl = std::make_unique <CWinSockets>();
#else
    m_SockImpl = std::make_unique <CLinuxSockets>();
#endif

}

CSocket::~CSocket()
{
    Shutdown();
}

bool CSocket::Init(const SAddress& Address)
{
    return m_SockImpl->Init(Address);
}
void CSocket::Shutdown()
{
    m_SockImpl->Shutdown();
}
void CSocket::Bind()
{
    m_SockImpl->Bind();
}
void CSocket::Listen()
{
    m_SockImpl->Listen();
}

}
