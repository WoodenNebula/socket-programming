#pragma once

#include "SocketInterface.h"
#include <WinSock2.h>

struct addrinfo;

namespace Sockets
{

using SNativeAddress = addrinfo;

class CWinSockets : public ISocket
{
public:
    CWinSockets() = default;
    virtual ~CWinSockets() override;

    ////////////////
    /// ISockets ///
    ////////////////
    virtual bool Init(const SAddress& Address) override;
    virtual void Shutdown() override;
    virtual void Bind() override;
    virtual void Listen() override;

private:
    bool m_bIsInitialized{ false };
    SAddress m_Address;

    SNativeAddress* m_NativeSocketAddr{ nullptr };
    SOCKET m_NativeSocket{ INVALID_SOCKET };
};
}   // namespace Sockets
