#pragma once

#include "SocketInterface.h"
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

private:
    bool m_bIsInitialized{ false };
    SAddress m_Address;

    SNativeAddress* m_NativeSocketAddr{ nullptr };
};
}   // namespace Sockets
