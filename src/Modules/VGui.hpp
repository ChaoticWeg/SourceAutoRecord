#pragma once
#include "Module.hpp"

#include "Features/Hud/InputHud.hpp"
#include "Features/Hud/InspectionHud.hpp"
#include "Features/Hud/SpeedrunHud.hpp"

#include "Interface.hpp"
#include "Utils.hpp"

class VGui : public Module {
public:
    Interface* enginevgui = nullptr;

    InputHud* inputHud = nullptr;
    SpeedrunHud* speedrunHud = nullptr;
    InspectionHud* inspectionHud = nullptr;

private:
    bool respectClShowPos = true;

public:
    // CEngineVGui::Paint
    DECL_DETOUR(Paint, int mode)

    bool Init() override;
    void Shutdown() override;
    const char* Name() override { return MODULE("engine"); }
};

extern VGui* vgui;
