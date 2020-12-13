#pragma once

#include "Usings.h"

class Clock
{
public:
    Clock();
    sf::Time getElapsedTime();
    sf::Time restart();
    sf::Time getCurrentTime();
    static float millsAsSeconds(sf::Time mils) { return mils / 1000.f; }
    static float secondsAsMills(float seconds) { return seconds * 1000; }
private:
    sf::Time m_startTime;
};

