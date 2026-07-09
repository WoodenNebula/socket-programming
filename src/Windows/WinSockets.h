#pragma once

#include "SocketInterface.h"
#define NOMINMAX
#include <WinSock2.h>

struct addrinfo;

namespace Sockets
{

using SNativeAddress = addrinfo;
using SNativeSocket = SOCKET;

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

    virtual void Close() override;

    /* Server-side interface */
    virtual void Bind() override;
    virtual void Listen() override;
    virtual std::unique_ptr<ISocket> Accept() override;

    /* Client-side interface */
    virtual void Connect() override;

    /* Comms */
    virtual bool Send(const SSocketPayload& Payload) override;
    virtual ERecieveResponse Receive(SSocketPayload& Payload) override;

private:
    bool m_bIsInitialized{ false };
    SAddress m_Address;

    SNativeAddress* m_NativeSocketAddr{ nullptr };
    SNativeSocket m_NativeSocket{ INVALID_SOCKET };
};
}   // namespace Sockets
