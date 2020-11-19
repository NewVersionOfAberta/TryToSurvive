#pragma once
#include <Windows.h>
#include "Window.h"
#include "EventManager.h"
#include "StateManager.h"
#include "Entity_Manager.h"
#include "System_Manager.h"
#include "Clock.h"
#include <iostream>

class Game {
public:
	Game();
	~Game();

	void Update();
	void Render();
	void LateUpdate();

	sf::Time GetElapsed();

	Window* GetWindow();
private:
	void RestartClock();
	Clock m_clock;
	sf::Time m_elapsed;
	SharedContext m_context;
	Window m_window;
	StateManager m_stateManager;
	EntityManager m_entityManager;
	SystemManager m_systemManager;
};

