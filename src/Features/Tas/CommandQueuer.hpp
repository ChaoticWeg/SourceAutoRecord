#pragma once
#include <string>
#include <vector>

#include "Features/Feature.hpp"

#include "Command.hpp"
#include "Variable.hpp"

struct CommandFrame {
    int framesLeft;
    std::string command;
    int splitScreen;
};

class CommandQueuer : public Feature {
public:
    std::vector<CommandFrame> frames;
    bool isRunning;
    int baseIndex;
    int curSplitScreen;
    int curDelay;

public:
    CommandQueuer();
    void AddFrame(int framesLeft, std::string command, bool relative = false);
    void AddFrames(int framesLeft, int interval, int lastFrame, std::string command, bool relative = false);
    void SetSplitScreen(int splitScreen);
    void Stop();
    void Reset();
    void Start();
    void DelayQueueBy(int frames);
};

extern CommandQueuer* tasQueuer;

extern Variable sar_tas_autostart;

extern Command sar_tas_frame_at;
extern Command sar_tas_frames_at;
extern Command sar_tas_frame_after;
extern Command sar_tas_frames_after;
extern Command sar_tas_start;
extern Command sar_tas_reset;
extern Command sar_tas_ss;
extern Command sar_tas_delay;
extern Command sar_tas_frame_at_for;
extern Command sar_tas_frame_after_for;
