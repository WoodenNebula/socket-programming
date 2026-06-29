#pragma once
#include <array>
#include <string>
#include <string_view>
#include <memory>

namespace Sockets
{
struct SAddress
{
    using IPv4_t = std::array<uint8_t, 4>;
    using Octet_t = uint8_t;
    using Port_t = uint16_t;

public:
    SAddress() :IPv4{ 0 }, Port{ 0 }
    {}
    SAddress(std::string_view Address)
    {
        const auto& [ipv4, port] = FromString(Address);
        IPv4 = ipv4;
        Port = port;
    }

    inline void Clear()
    {
        IPv4 = { 0 }; Port = 0;
    }


/// <summary>
/// Returns `true` if Host has a non-zero address i.e. if Host != 0.0.0.0
/// </summary>
    bool IsValid() const;
    operator bool() const
    {
        return IsValid();
    }

/// <summary>
/// Builds a full IPv4 socket address in form Host:Port i.e. '127.0.0.1:6969'
/// </summary>
/// <returns>std::string(Host:Port) i.e. "127.0.0.1:6969" </returns>
    std::string ToString() const;
public:
    IPv4_t IPv4;
    Port_t Port;

public:
    static inline constexpr Port_t DefaultServerPort()
    {
        return 27020;
    }
    static inline constexpr std::string_view DefaultServerAddress()
    {
        return "127.0.0.1:27020";
    }

    static inline std::string GetPortAsString(Port_t Port)
    {
        return std::to_string(Port);
    }
    static inline std::string GetAddressAsString(const IPv4_t& IP)
    {
        std::string ip{ "" };
        for (const Octet_t& octet : IP)
            ip += std::to_string(octet) + ".";
        ip.pop_back();
        return ip;
    }

    static bool IsPortInRange(int Port);
    static bool IsOctetValid(int Octet, char Separator = '.');

    static SAddress FromString(std::string_view Address);
};


class ISocket;

class CSocket
{
public:
    CSocket();
    ~CSocket();

    virtual bool Init(const SAddress& Address);
    virtual void Shutdown();
    virtual void Bind();
    virtual void Listen();

private:
    std::unique_ptr<ISocket> m_SockImpl;
};
} // namespace Sockets
