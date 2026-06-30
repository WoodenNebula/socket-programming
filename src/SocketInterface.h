#pragma once

#include "Sockets.h"

namespace Sockets
{

class ISocket
{
public:
    virtual ~ISocket() = default;

    virtual bool Init(const SAddress& Address) = 0;
    virtual void Shutdown() = 0;

    virtual void Send() = 0;
    virtual void Close() = 0;

    /* Server-side interface */
    virtual void Bind() = 0;
    virtual void Listen() = 0;
    virtual void Accept() = 0;

    /* Client-side interface */
    virtual void Connect() = 0;
};

}   // namespace Sockets
