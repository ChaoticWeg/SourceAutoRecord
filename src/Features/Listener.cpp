#include "Listener.hpp"

#include <cstring>

#include "Features/Session.hpp"
#include "Features/Speedrun/SpeedrunTimer.hpp"
#include "Features/Timer/Timer.hpp"

#include "Modules/Engine.hpp"

#include "Utils/SDK.hpp"

#include "Variable.hpp"

Variable sar_debug_game_events("sar_debug_game_events", "0", "Prints game event data, similar to net_showevents.\n");

Listener* listener;

Listener::Listener()
    : m_bRegisteredForEvents(false)
{
    this->hasLoaded = true;
}
void Listener::Init()
{
    if (engine->hasLoaded && engine->AddListener && !this->m_bRegisteredForEvents) {
        for (const auto& event : EVENTS) {
            this->m_bRegisteredForEvents = engine->AddListener(engine->s_GameEventManager->ThisPtr(),
                this, event, true);

            if (this->m_bRegisteredForEvents) {
                //console->DevMsg("SAR: Added event listener for %s!\n", event);
            } else {
                console->DevWarning("SAR: Failed to add event listener for %s!\n", event);
                break;
            }
        }
    }
}
void Listener::Shutdown()
{
    if (engine->hasLoaded && engine->RemoveListener && this->m_bRegisteredForEvents) {
        engine->RemoveListener(engine->s_GameEventManager->ThisPtr(), this);
        this->m_bRegisteredForEvents = false;
    }
}
Listener::~Listener()
{
    this->Shutdown();
}
void Listener::FireGameEvent(IGameEvent* ev)
{
    if (!ev)
        return;

    if (sar_debug_game_events.GetBool()) {
        console->Print("[%i] Event fired: %s\n", engine->GetSessionTick(), ev->GetName());
        if (engine->ConPrintEvent) {
#ifdef _WIN32
            engine->ConPrintEvent(ev);
#else
            engine->ConPrintEvent(engine->s_GameEventManager->ThisPtr(), ev);
#endif
        }
    }

    if (engine->GetMaxClients() >= 2) {
        // TODO: Start when orange spawns?
        if (!std::strcmp(ev->GetName(), "player_spawn_orange")) {
            console->Print("Detected cooperative spawn!\n");
            session->Rebase(*engine->tickcount);
            timer->Rebase(*engine->tickcount);
            speedrun->Unpause(engine->tickcount);
        }
    }
}
int Listener::GetEventDebugID()
{
    return 42;
}
void Listener::DumpGameEvents()
{
    if (!engine->s_GameEventManager) {
        return;
    }

    auto s_GameEventManager = reinterpret_cast<uintptr_t>(engine->s_GameEventManager->ThisPtr());
    auto m_Size = *reinterpret_cast<int*>(s_GameEventManager + CGameEventManager_m_Size);
    console->Print("m_Size = %i\n", m_Size);
    if (m_Size > 0) {
        auto m_GameEvents = *(uintptr_t*)(s_GameEventManager + CGameEventManager_m_GameEvents);
        for (int i = 0; i < m_Size; i++) {
            auto name = *(char**)(m_GameEvents + CGameEventDescriptor_Size * i + CGameEventManager_m_GameEvents);
            console->Print("%s\n", name);
        }
    }
}
