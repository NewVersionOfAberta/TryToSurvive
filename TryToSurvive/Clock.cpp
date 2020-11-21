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

sf::Time Clock::getElapsedTime()
{
    INT64 now = getCurrentTime();
    INT64 elapsed = now - m_startTime;

    return elapsed;
}

sf::Time Clock::restart()
{
    INT64 now = getCurrentTime();
    INT64 elapsed = now - m_startTime;
    m_startTime = now;

    return elapsed;
}

sf::Time Clock::getCurrentTime()
{
    static LARGE_INTEGER frequency = getFrequency();
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    return 1000000 * time.QuadPart / frequency.QuadPart;

}

