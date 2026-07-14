#pragma once

#include "Sockets.h"


namespace Sockets
{
struct SAddress;
struct SSocketPayload;

class ISocket
{
public:
    virtual ~ISocket() = default;

    virtual bool Init(const SAddress& Address) = 0;
    virtual void Shutdown() = 0;

    virtual void Close() = 0;

    /* Server-side interface */
    virtual void Bind() = 0;
    virtual void Listen() = 0;
    virtual std::unique_ptr<ISocket> Accept() = 0;

    /* Client-side interface */
    virtual void Connect() = 0;

    /* Comms */
    virtual bool Send(const SSocketPayload& Payload) = 0;
    virtual ERecieveResponse Receive(SSocketPayload& Payload) = 0;
};

}   // namespace Sockets
