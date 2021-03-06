#pragma once
#include "Module.hpp"

#include "EngineDemoPlayer.hpp"
#include "EngineDemoRecorder.hpp"

#include "Command.hpp"
#include "Interface.hpp"
#include "Utils.hpp"

#if _WIN32
#define IServerMessageHandler_VMT_Offset 8
#endif

struct cmdalias_t {
    cmdalias_t* next;
    char name[32];
    char* value;
};

class Engine : public Module {
public:
    Interface* engineClient = nullptr;
    Interface* cl = nullptr;
    Interface* s_GameEventManager = nullptr;
    Interface* eng = nullptr;
    Interface* debugoverlay = nullptr;

    using _ClientCmd = int(__func*)(void* thisptr, const char* szCmdString);
    using _GetLocalPlayer = int(__func*)(void* thisptr);
    using _GetViewAngles = int(__func*)(void* thisptr, QAngle& va);
    using _SetViewAngles = int(__func*)(void* thisptr, QAngle& va);
    using _GetMaxClients = int (*)();
    using _GetGameDirectory = char*(__cdecl*)();
    using _AddListener = bool(__func*)(void* thisptr, IGameEventListener2* listener, const char* name, bool serverside);
    using _RemoveListener = bool(__func*)(void* thisptr, IGameEventListener2* listener);
    using _Cbuf_AddText = void(__cdecl*)(int slot, const char* pText, int nTickDelay);
    using _AddText = void(__func*)(void* thisptr, const char* pText, int nTickDelay);
#ifdef _WIN32
    using _GetScreenSize = int(__stdcall*)(int& width, int& height);
    using _GetActiveSplitScreenPlayerSlot = int (*)();
    using _ScreenPosition = int(__stdcall*)(const Vector& point, Vector& screen);
    using _ConPrintEvent = int(__stdcall*)(IGameEvent* ev);
#else
    using _GetScreenSize = int(__cdecl*)(void* thisptr, int& width, int& height);
    using _GetActiveSplitScreenPlayerSlot = int (*)(void* thisptr);
    using _ScreenPosition = int(__stdcall*)(void* thisptr, const Vector& point, Vector& screen);
    using _ConPrintEvent = int(__cdecl*)(void* thisptr, IGameEvent* ev);
#endif

    _GetScreenSize GetScreenSize = nullptr;
    _ClientCmd ClientCmd = nullptr;
    _GetLocalPlayer GetLocalPlayer = nullptr;
    _GetViewAngles GetViewAngles = nullptr;
    _SetViewAngles SetViewAngles = nullptr;
    _GetMaxClients GetMaxClients = nullptr;
    _GetGameDirectory GetGameDirectory = nullptr;
    _GetActiveSplitScreenPlayerSlot GetActiveSplitScreenPlayerSlot = nullptr;
    _AddListener AddListener = nullptr;
    _RemoveListener RemoveListener = nullptr;
    _Cbuf_AddText Cbuf_AddText = nullptr;
    _AddText AddText = nullptr;
    _ScreenPosition ScreenPosition = nullptr;
    _ConPrintEvent ConPrintEvent = nullptr;

    EngineDemoPlayer* demoplayer = nullptr;
    EngineDemoRecorder* demorecorder = nullptr;

    int* tickcount = nullptr;
    float* interval_per_tick = nullptr;
    char* m_szLevelName = nullptr;
    bool* m_bLoadgame = nullptr;
    CHostState* hoststate = nullptr;
    void* s_CommandBuffer = nullptr;
    bool* m_bWaitEnabled = nullptr;
    cmdalias_t* cmd_alias = nullptr;

public:
    void ExecuteCommand(const char* cmd);
    void ClientCommand(const char* fmt, ...);
    int GetSessionTick();
    float ToTime(int tick);
    int GetLocalPlayerIndex();
    QAngle GetAngles();
    void SetAngles(QAngle va);
    void SendToCommandBuffer(const char* text, int delay);
    int PointToScreen(const Vector& point, Vector& screen);
    void SafeUnload(const char* postCommand = nullptr);

    // CClientState::Disconnect
    DECL_DETOUR(Disconnect, bool bShowMainMenu)
#ifdef _WIN32
    DECL_DETOUR(Disconnect2, int unk1, int unk2, int unk3)
    DECL_DETOUR_COMMAND(connect)
#else
    DECL_DETOUR(Disconnect2, int unk, bool bShowMainMenu)
#endif

    // CClientState::SetSignonState
    DECL_DETOUR(SetSignonState, int state, int count, void* unk)
    DECL_DETOUR(SetSignonState2, int state, int count)

    // CEngine::Frame
    DECL_DETOUR(Frame)

    DECL_DETOUR_COMMAND(plugin_load)
    DECL_DETOUR_COMMAND(plugin_unload)
    DECL_DETOUR_COMMAND(exit)
    DECL_DETOUR_COMMAND(quit)
    DECL_DETOUR_COMMAND(help)

#ifdef _WIN32
    using _ReadCustomData = int(__fastcall*)(void* thisptr, int edx, void* unk1, void* unk2);
    static _ReadCustomData ReadCustomData;

    // CDemoSmootherPanel::ParseSmoothingInfo
    static uintptr_t ParseSmoothingInfo_Skip;
    static uintptr_t ParseSmoothingInfo_Default;
    static uintptr_t ParseSmoothingInfo_Continue;
    DECL_DETOUR_MID_MH(ParseSmoothingInfo_Mid)

    Memory::Patch* demoSmootherPatch;
#endif

    bool Init() override;
    void Shutdown() override;
    const char* Name() override { return MODULE("engine"); }
};

extern Engine* engine;
