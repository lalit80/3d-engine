#ifndef TIMER_H
#define TIMER_H

#include <windows.h>

class Timer
{
public:
    static void Init();
    static void Tick();

    static float GetDeltaTime();
    static float GetTime();

private:
    static LARGE_INTEGER frequency;
    static LARGE_INTEGER lastTime;
    static float deltaTime;
    static float totalTime;
};

#endif
