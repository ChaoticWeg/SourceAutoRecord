#include "Portal.hpp"

#include "Game.hpp"
#include "Offsets.hpp"

Portal::Portal()
{
    this->version = SourceGame_Portal;
}
void Portal::LoadOffsets()
{
    HalfLife2::LoadOffsets();

    using namespace Offsets;

    // engine.dll

    Cbuf_AddText = 60; // CEngineClient::ClientCmd
    s_CommandBuffer = 71; // Cbuf_AddText
    AddText = 76; // Cbuf_AddText
    tickcount = 98; // CClientState::ProcessTick
    interval_per_tick = 68; // CClientState::ProcessTick
    HostState_OnClientConnected = 570; // CClientState::SetSignonState
    demoplayer = 115; // CClientState::Disconnect
    demorecorder = 128; // CClientState::Disconnect
    m_szLevelName = 34; // CEngineTool::GetCurrentMap
    //FireEventIntern = 12; // CGameEventManager::FireEventClientSide
    //ConPrintEvent = 262; // CGameEventManager::FireEventIntern
    AutoCompletionFunc = 66; // listdemo_CompletionFunc
    Key_SetBinding = 135; // unbind

    // server.dll

    ServiceEventQueue = 152; // CServerGameDLL::GameFrame
    UTIL_PlayerByIndex = 39; // CServerGameDLL::Think
    iNumPortalsPlaced = 4796; // CPortal_Player::IncrementPortalsPlaced

    // vguimatsurface.dll

    StartDrawing = 129; // CMatSystemSurface::PaintTraverseEx
    FinishDrawing = 650; // CMatSystemSurface::PaintTraverseEx
}
const char* Portal::Version()
{
    return "Portal (1910503)";
}
const char* Portal::Process()
{
    return "hl2.exe";
}
