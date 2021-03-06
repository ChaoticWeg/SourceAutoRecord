#pragma once
#include "Hud.hpp"

#include "Variable.hpp"

class SpeedrunHud : public Hud {
public:
    bool GetCurrentSize(int& xSize, int& ySize) override;
    void Draw() override;
};

extern Variable sar_sr_hud;
extern Variable sar_sr_hud_x;
extern Variable sar_sr_hud_y;
extern Variable sar_sr_hud_font_color;
extern Variable sar_sr_hud_font_index;
