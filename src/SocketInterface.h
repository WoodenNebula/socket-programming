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

    virtual void Bind() = 0;
    virtual void Listen() = 0;
};

}   // namespace Sockets
