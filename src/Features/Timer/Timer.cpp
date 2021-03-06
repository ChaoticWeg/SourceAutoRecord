#include "Timer.hpp"

#include "TimerAverage.hpp"
#include "TimerCheckPoints.hpp"

#include "Features/Stats/Stats.hpp"

#include "Modules/Console.hpp"
#include "Modules/Engine.hpp"

#include "Command.hpp"
#include "Utils.hpp"
#include "Variable.hpp"

Variable sar_timer_always_running("sar_timer_always_running", "1", "Timer will save current value when disconnecting.\n");

Timer* timer;

Timer::Timer()
    : isRunning(false)
    , isPaused(true)
    , baseTick(0)
    , totalTicks(0)
{
    this->avg = new TimerAverage();
    this->cps = new TimerCheckPoints();

    this->hasLoaded = this->avg && this->cps;
}
Timer::~Timer()
{
    SAFE_DELETE(this->avg)
    SAFE_DELETE(this->cps)
}
void Timer::Start(int engineTick)
{
    this->isRunning = true;
    this->isPaused = false;
    this->baseTick = engineTick;
    this->totalTicks = 0;
}
void Timer::Rebase(int engineTick)
{
    if (!this->isRunning)
        return;
    this->baseTick = engineTick;
    this->isPaused = false;
}
int Timer::GetTick(int engineTick)
{
    if (!this->isRunning)
        return this->totalTicks;
    int tick = engineTick - this->baseTick;
    return (tick >= 0) ? tick + this->totalTicks : this->totalTicks;
}
void Timer::Save(int engineTick)
{
    this->isPaused = true;
    this->totalTicks = this->GetTick(engineTick);
}
void Timer::Stop(int engineTick)
{
    this->Save(engineTick);
    this->isRunning = false;
}

// Commands

CON_COMMAND(sar_timer_start, "Starts timer.\n")
{
    if (timer->isRunning)
        console->DevMsg("Restarting timer!\n");
    else
        console->DevMsg("Starting timer!\n");
    timer->Start(*engine->tickcount);

    if (sar_stats_auto_reset.GetInt() >= 2) {
        stats->ResetAll();
    }
}
CON_COMMAND(sar_timer_stop, "Stops timer.\n")
{
    if (!timer->isRunning) {
        console->DevMsg("Timer isn't running!\n");
        return;
    }

    timer->Stop(*engine->tickcount);

    if (timer->avg->isEnabled) {
        int tick = timer->GetTick(*engine->tickcount);
        timer->avg->Add(tick, engine->ToTime(tick), engine->m_szLevelName);
    }
}
CON_COMMAND(sar_timer_result, "Prints result of timer.\n")
{
    int tick = timer->GetTick(*engine->tickcount);
    float time = engine->ToTime(tick);

    if (timer->isRunning) {
        console->PrintActive("Result: %i (%.3f)\n", tick, time);
    } else {
        console->Print("Result: %i (%.3f)\n", tick, time);
    }
}
CON_COMMAND(sar_avg_start, "Starts calculating the average when using timer.\n")
{
    timer->avg->Start();
}
CON_COMMAND(sar_avg_stop, "Stops average calculation.\n")
{
    timer->avg->isEnabled = false;
}
CON_COMMAND(sar_avg_result, "Prints result of average.\n")
{
    int average = timer->avg->items.size();
    if (average > 0) {
        console->Print("Average of %i:\n", average);
    } else {
        console->Print("No result!\n");
        return;
    }

    for (int i = 0; i < average; i++) {
        console->Print("%s -> ", timer->avg->items[i].map);
        console->Print("%i ticks", timer->avg->items[i].ticks);
        console->Print("(%.3f)\n", timer->avg->items[i].time);
    }

    if (timer->isRunning) {
        console->PrintActive("Result: %i (%.3f)\n", timer->avg->averageTicks, timer->avg->averageTime);
    } else {
        console->Print("Result: %i (%.3f)\n", timer->avg->averageTicks, timer->avg->averageTime);
    }
}
CON_COMMAND(sar_cps_add, "Saves current time of timer.\n")
{
    if (!timer->isRunning) {
        console->DevMsg("Timer isn't running!\n");
        return;
    }

    int tick = timer->GetTick(engine->GetSessionTick());
    timer->cps->Add(tick, engine->ToTime(tick), engine->m_szLevelName);
}
CON_COMMAND(sar_cps_clear, "Resets saved times of timer.\n")
{
    timer->cps->Reset();
}
CON_COMMAND(sar_cps_result, "Prints result of timer checkpoints.\n")
{
    int cps = timer->cps->items.size();
    if (cps > 0) {
        console->Print("Result of %i checkpoint%s:\n", cps, (cps == 1) ? "" : "s");
    } else {
        console->Print("No result!\n");
        return;
    }

    for (int i = 0; i < cps; i++) {
        if (i == cps - 1 && timer->isRunning) {
            console->PrintActive("%s -> ", timer->cps->items[i].map);
            console->PrintActive("%i ticks", timer->cps->items[i].ticks);
            console->PrintActive("(%.3f)\n", timer->cps->items[i].time);
        } else {
            console->Print("%s -> ", timer->cps->items[i].map);
            console->Print("%i ticks", timer->cps->items[i].ticks);
            console->Print("(%.3f)\n", timer->cps->items[i].time);
        }
    }

    if (!timer->isRunning) {
        int tick = timer->GetTick(*engine->tickcount);
        float time = engine->ToTime(tick);
        console->Print("Result: %i (%.3f)\n", tick, time);
    }
}
