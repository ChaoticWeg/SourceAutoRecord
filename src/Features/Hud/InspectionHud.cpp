#include "InspectionHud.hpp"

#include "Features/Routing/EntityInspector.hpp"

#include "Modules/Engine.hpp"
#include "Modules/Scheme.hpp"
#include "Modules/Surface.hpp"

#include "Variable.hpp"

Variable sar_ei_hud("sar_ei_hud", "0", 0, "Draws entity inspection data.\n");
Variable sar_ei_hud_x("sar_ei_hud_x", "0", 0, "X offset of entity inspection HUD.\n");
Variable sar_ei_hud_y("sar_ei_hud_y", "0", 0, "Y offset of entity inspection HUD.\n");
Variable sar_ei_hud_z("sar_ei_hud_z", "0", 0, "Z offset of entity inspection HUD.\n");
Variable sar_ei_hud_font_color("sar_ei_hud_font_color", "255 255 255 255", "RGBA font color of entity inspection HUD when not recording.\n", 0);
Variable sar_ei_hud_font_color2("sar_ei_hud_font_color2", "153 23 9 255", "RGBA font color of entity inspection HUD when recording.\n", 0);
Variable sar_ei_hud_font_index("sar_ei_hud_font_index", "1", 0, "Font index of entity inspection HUD.\n");

bool InspectionHud::GetCurrentSize(int& xSize, int& ySize)
{
    return false;
}
void InspectionHud::Draw()
{
    auto mode = sar_ei_hud.GetInt();
    if (mode == 0)
        return;

    surface->StartDrawing(surface->matsurface->ThisPtr());

    auto font = scheme->GetDefaultFont() + sar_ei_hud_font_index.GetInt();

    auto fontColorStr = !inspector->IsRunning()
        ? sar_ei_hud_font_color.GetString()
        : sar_ei_hud_font_color2.GetString();
    auto fontColor = this->GetColor(fontColorStr);

    auto data = inspector->GetData();

    auto offset = Vector {
        (float)sar_ei_hud_x.GetInt(),
        (float)sar_ei_hud_y.GetInt(),
        (float)sar_ei_hud_z.GetInt()
    };

    auto screen = Vector();
    engine->PointToScreen(data.origin + offset, screen);

    switch (mode) {
    case 2:
        surface->DrawTxt(font, screen.x, screen.y, fontColor, "ang: %.3f %.3f %.3f",
            data.angles.x, data.angles.y, data.angles.z);
        break;
    case 3:
        surface->DrawTxt(font, screen.x, screen.y, fontColor, "vel: %.3f %.3f %.3f (%.3f)",
            data.velocity.x, data.velocity.y, data.velocity.z, data.velocity.Length());
        break;
    case 4:
        surface->DrawTxt(font, screen.x, screen.y, fontColor, "flags: %i", data.flags);
        break;
    case 5:
        surface->DrawTxt(font, screen.x, screen.y, fontColor, "eflags: %i", data.eFlags);
        break;
    case 6:
        surface->DrawTxt(font, screen.x, screen.y, fontColor, "maxspeed: %.3f", data.maxSpeed);
        break;
    case 7:
        surface->DrawTxt(font, screen.x, screen.y, fontColor, "gravity: %.3f", data.gravity);
        break;
    case 8:
        surface->DrawTxt(font, screen.x, screen.y, fontColor, "voffset: %.3f %.3f %.3f",
            data.viewOffset.x, data.viewOffset.y, data.viewOffset.z);
        break;
    default:
        surface->DrawTxt(font, screen.x, screen.y, fontColor, "pos: %.3f %.3f %.3f",
            data.origin.x, data.origin.y, data.origin.z);
        break;
    }

    surface->FinishDrawing();
}
