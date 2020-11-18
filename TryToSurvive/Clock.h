#pragma once
class Clock
{
public:
    Clock();
    INT64 getElapsedTime();
    INT64 restart();
    INT64 getCurrentTime();

private:
    INT64 m_startTime;
};

