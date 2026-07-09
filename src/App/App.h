#pragma once

#include "Sockets.h"
#include <string>

enum class ERoomState
{
    None = 0,
    Host,
    Client
};

class CApp
{
public:
    CApp();
    ~CApp();
    bool Init(ERoomState RoomState = ERoomState::None, std::string_view Username = "");
    void Run();

public:
    static ERoomState PromptHostOrJoin();
    static std::string PromptUsername();
protected:
    void Host();
    void Join();

    void HandleConnection(std::unique_ptr<Sockets::CSocket> Connection);

    void PrintRoomAddress(const Sockets::SAddress SelfAddress) const;

protected:
    static Sockets::SAddress PromptRoomAddress();



private:
    ERoomState m_RoomState{ ERoomState::None };
    std::string m_Username;

    static inline constexpr Sockets::SAddress::Port_t GetDefaultPort()
    {
        return 27020;
    }

};
