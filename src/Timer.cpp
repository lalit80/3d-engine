#include "Timer.h"

LARGE_INTEGER Timer::frequency;
LARGE_INTEGER Timer::lastTime;
float Timer::deltaTime = 0.0f;
float Timer::totalTime = 0.0f;

void Timer::Init()
{
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&lastTime);
}

void Timer::Tick()
{
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);

    deltaTime = (float)(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;

    lastTime = currentTime;
    totalTime += deltaTime;
}

float Timer::GetDeltaTime()
{
    return deltaTime;
}

float Timer::GetTime()
{
    return totalTime;
}
