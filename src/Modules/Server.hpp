#pragma once
#include "Module.hpp"

#include "Interface.hpp"
#include "Offsets.hpp"
#include "Utils.hpp"
#include "Variable.hpp"

#ifdef _WIN32
#define AirMove_Mid_Offset 679
#define AirMove_Signature "F3 0F 10 50 40"
#define AirMove_Continue_Offset 5
#define AirMove_Skip_Offset 142
#endif

class Server : public Module {
public:
    Interface* g_GameMovement = nullptr;
    Interface* g_ServerGameDLL = nullptr;

    using _UTIL_PlayerByIndex = void*(__cdecl*)(int index);
    using _GetAllServerClasses = ServerClass*(*)();

    _UTIL_PlayerByIndex UTIL_PlayerByIndex = nullptr;
    _GetAllServerClasses GetAllServerClasses = nullptr;

    CGlobalVars* gpGlobals = nullptr;
    bool* g_InRestore = nullptr;
    CEventQueue* g_EventQueue = nullptr;
    CEntInfo* m_EntPtrArray = nullptr;

private:
    bool jumpedLastTime = false;
    bool callFromCheckJumpButton = false;

public:
    void* GetPlayer();
    int GetPortals();
    CEntInfo* GetEntityInfoByIndex(int index);
    CEntInfo* GetEntityInfoByName(const char* name);
    CEntInfo* GetEntityInfoByClassName(const char* name);
    char* GetEntityName(void* entity);
    char* GetEntityClassName(void* entity);
    Vector GetAbsOrigin(void* entity);
    QAngle GetAbsAngles(void* entity);
    Vector GetLocalVelocity(void* entity);
    int GetFlags(void* entity);
    int GetEFlags(void* entity);
    float GetMaxSpeed(void* entity);
    float GetGravity(void* entity);
    Vector GetViewOffset(void* entity);
    void GetOffset(const char* className, const char* propName, int& offset);

private:
    int16_t FindOffset(SendTable* table, const char* propName);

public:
    // CGameMovement::CheckJumpButton
    DECL_DETOUR_T(bool, CheckJumpButton)

    static _CheckJumpButton CheckJumpButtonBase;

    // CGameMovement::PlayerMove
    DECL_DETOUR(PlayerMove)

    // CGameMovement::FinishGravity
    DECL_DETOUR(FinishGravity)

    // CGameMovement::AirMove
    DECL_DETOUR_B(AirMove)

#ifdef _WIN32
    // CGameMovement::AirMove
    static uintptr_t AirMove_Skip;
    static uintptr_t AirMove_Continue;
    DECL_DETOUR_MID_MH(AirMove_Mid)
#endif

// CServerGameDLL::GameFrame
#ifdef _WIN32
    DECL_DETOUR_STD(void, GameFrame, bool simulating)
#else
    DECL_DETOUR(GameFrame, bool simulating)
#endif

    bool Init() override;
    void Shutdown() override;
    const char* Name() override { return MODULE("server"); }
};

extern Server* server;

extern Variable sv_cheats;
extern Variable sv_footsteps;
extern Variable sv_alternateticks;
extern Variable sv_bonus_challenge;
extern Variable sv_accelerate;
extern Variable sv_airaccelerate;
extern Variable sv_friction;
extern Variable sv_maxspeed;
extern Variable sv_stopspeed;
extern Variable sv_maxvelocity;
extern Variable sv_transition_fade_time;
extern Variable sv_laser_cube_autoaim;
