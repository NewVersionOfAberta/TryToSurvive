#include <Windows.h>
#include "Clock.h"


LARGE_INTEGER getFrequency()
{
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    return frequency;
}

Clock::Clock() : m_startTime(getCurrentTime())
{
}

INT64 Clock::getElapsedTime()
{
    INT64 now = getCurrentTime();
    INT64 elapsed = now - m_startTime;

    return elapsed;
}

INT64 Clock::restart()
{
    INT64 now = getCurrentTime();
    INT64 elapsed = now - m_startTime;
    m_startTime = now;

    return elapsed;
}

INT64 Clock::getCurrentTime()
{
    static LARGE_INTEGER frequency = getFrequency();
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    return 1000000 * time.QuadPart / frequency.QuadPart;

}
