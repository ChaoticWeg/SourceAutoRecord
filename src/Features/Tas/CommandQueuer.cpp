#include "CommandQueuer.hpp"

#include <algorithm>
#include <string>

#include "Modules/Console.hpp"
#include "Modules/Engine.hpp"

#include "Command.hpp"
#include "Variable.hpp"

Variable sar_tas_autostart("sar_tas_autostart", "1", "Starts queued commands automatically on first frame after a load.\n");

CommandQueuer* tasQueuer;

CommandQueuer::CommandQueuer()
    : frames()
    , isRunning(false)
    , baseIndex(0)
    , curSplitScreen(0)
    , curDelay(0)
{
    this->hasLoaded = true;
}
void CommandQueuer::AddFrame(int framesLeft, std::string command, bool relative)
{
    if (relative) {
        framesLeft += this->baseIndex;
    } else {
        this->baseIndex = framesLeft;
    }

    this->frames.push_back(CommandFrame{
        framesLeft + this->curDelay,
        command,
        this->curSplitScreen });
}
void CommandQueuer::AddFrames(int framesLeft, int interval, int lastFrame, std::string command, bool relative)
{
    if (relative) {
        framesLeft += baseIndex;
        lastFrame += baseIndex;
    } else {
        baseIndex = framesLeft;
    }

    for (; framesLeft <= lastFrame; framesLeft += interval) {
        this->frames.push_back(CommandFrame{
            framesLeft + this->curDelay,
            command,
            this->curSplitScreen });
    }
}
void CommandQueuer::SetSplitScreen(int splitScreen)
{
    this->curSplitScreen = splitScreen;
}
void CommandQueuer::Stop()
{
    this->isRunning = false;
}
void CommandQueuer::Reset()
{
    this->Stop();
    this->baseIndex = 0;
    this->frames.clear();
    this->curDelay = 0;
}
void CommandQueuer::Start()
{
    if (!this->frames.empty()) {
        std::sort(this->frames.begin(), this->frames.end(), [](const auto& a, const auto& b) {
            return a.framesLeft < b.framesLeft;
        });
        this->isRunning = true;
    }
}
void CommandQueuer::DelayQueueBy(int frames)
{
    this->curDelay = frames;
}

// Commands

CON_COMMAND(sar_tas_frame_at,
    "Adds command frame to the queue at specified frame.\n"
    "Usage: sar_tas_frame_at <frame> [command_to_execute]\n")
{
    if (args.ArgC() != 3) {
        return console->Print("sar_tas_frame_at <frame> [command_to_execute] : Adds command frame to the queue.\n");
    }

    tasQueuer->AddFrame(std::atoi(args[1]), std::string(args[2]));
}
CON_COMMAND(sar_tas_frames_at,
    "Adds command frame multiple times to the queue at specified frame.\n"
    "Usage: sar_tas_frames_at <frame> <interval> <last_frame> [command_to_execute]\n")
{
    if (args.ArgC() != 5) {
        return console->Print("sar_tas_frames_at <frame> <interval> <last_frame> [command_to_execute] : "
            "Adds command frame multiple times to the queue.\n");
    }

    tasQueuer->AddFrames(std::atoi(args[1]), std::atoi(args[2]), std::atoi(args[3]), std::string(args[4]));
}
CON_COMMAND(sar_tas_frame_after,
    "Adds command frame to the queue after waiting for specified amount of frames.\n"
    "Usage: sar_tas_frame_after <frames_to_wait> [command_to_execute]\n")
{
    if (args.ArgC() != 3) {
        return console->Print("sar_tas_frame_after <frames_to_wait> [command_to_execute] : "
            "Adds command frame to the queue.\n");
    }

    tasQueuer->AddFrame(std::atoi(args[1]), std::string(args[2]), true);
}
CON_COMMAND(sar_tas_frames_after,
    "Adds command frame multiple times to the queue after waiting for specified amount of frames.\n"
    "Usage: sar_tas_frames_after <frames_to_wait> <interval> <length> [command_to_execute]\n")
{
    if (args.ArgC() != 5) {
        return console->Print("sar_tas_frames_after <frames_to_wait> <interval> <length> [command_to_execute] : "
            "Adds command frame multiple times to the queue.\n");
    }

    tasQueuer->AddFrames(std::atoi(args[1]), std::atoi(args[2]), std::atoi(args[3]), std::string(args[4]), true);
}
CON_COMMAND(sar_tas_start, "Starts executing queued commands.\n")
{
    tasQueuer->Start();
}
CON_COMMAND(sar_tas_reset, "Stops executing commands and clears them from the queue.\n")
{
    tasQueuer->Reset();
}
CON_COMMAND(sar_tas_ss, "Select split screen index for command buffer (0 or 1).\n")
{
    if (args.ArgC() != 2) {
        return console->Print("sar_tas_ss <index> : Select split screen index for command buffer (0 or 1).\n");
    }

    auto index = std::atoi(args[1]);
    if (index == 0 || index == 1) {
        tasQueuer->SetSplitScreen(index);
    } else {
        console->Print("Invalid split screen index!\n");
    }
}
CON_COMMAND(sar_tas_delay, "Delays command queue by specified amount of frames.\n")
{
    if (args.ArgC() != 2) {
        return console->Print("sar_tas_delay <frames_to_wait> : Delays command queue by specified amount of frames.\n");
    }

    tasQueuer->DelayQueueBy(std::atoi(args[1]));
}
CON_COMMAND(sar_tas_frame_at_for,
    "Adds two command frames to the queue at specified frame, the last frame will be executed after a delay.\n"
    "Usage: sar_tas_frame_at_for <frame> <delay> <first_command> <last_command>\n")
{
    if (args.ArgC() != 5) {
        return console->Print("sar_tas_frame_at_for <frame> <delay> <first_command> <last_command> : "
            "Adds two command frames to the queue at specified frame, the last frame will be executed after a delay.\n");
    }

    tasQueuer->AddFrame(std::atoi(args[1]), std::string(args[3]));
    tasQueuer->AddFrame(std::atoi(args[1]) + std::atoi(args[2]), std::string(args[4]));
}
CON_COMMAND(sar_tas_frame_after_for,
    "Adds two command frames to the queue after waiting for specified amount of frames, "
    "the last frame will be executed after a delay.\n"
    "Usage: sar_tas_frame_after_for <frames_to_wait> <delay> <first_command> <last_command>\n")
{
    if (args.ArgC() != 5) {
        return console->Print("sar_tas_frame_after_for <frames_to_wait> <delay> <first_command> <last_command> : "
            "Adds two command frames to the queue at specified frame, the last frame will be executed after a delay.\n");
    }

    tasQueuer->AddFrame(std::atoi(args[1]), std::string(args[3]), true);
    tasQueuer->AddFrame(std::atoi(args[1]) + std::atoi(args[2]), std::string(args[4]), true);
}
