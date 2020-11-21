#pragma once

#include "Usings.h"

class Clock
{
public:
    Clock();
    sf::Time getElapsedTime();
    sf::Time restart();
    sf::Time getCurrentTime();
    sf::Uint32 asSeconds();

private:
    sf::Time m_startTime;
};

