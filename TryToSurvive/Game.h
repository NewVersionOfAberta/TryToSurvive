#pragma once
#include <Windows.h>
#include <iostream>
#include "Window.h"
#include "EventManager.h"
#include "StateManager.h"
#include "Entity_Manager.h"
#include "System_Manager.h"
#include "Clock.h"


class Game {
public:
	Game(HWND l_window);
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
	ClientEntityManager m_entityManager;
	ClientSystemManager m_systemManager;
	TextureManager m_textureManager;
};

