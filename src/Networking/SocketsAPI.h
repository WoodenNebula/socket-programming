#pragma once

namespace Sockets
{
class CSocketAPI
{
public:
    CSocketAPI() = delete;
public:
    static bool Init();
    static void Shutdown();
protected:
    static inline bool s_IsInitialized = false;
};
}
