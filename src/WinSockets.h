#pragma once

#include "SocketInterface.h"

namespace Sockets
{

class CWinSockets : public ISocket
{
public:
    CWinSockets() = default;
    virtual ~CWinSockets() override;

    ////////////////
    /// ISockets ///
    ////////////////
    virtual bool Init() override;
    virtual void Shutdown() override;

private:
    bool m_bIsInitialized{ false };

};
}   // namespace Sockets
