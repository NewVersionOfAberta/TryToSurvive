#pragma once
#include "Window.h"
#include "EventManager.h"
#include "Client_System_Manager.h"
#include "Client_Entity_Manager.h"
#include "TextureManager.h"
#include "Client.h"

class Map;

struct SharedContext {
	SharedContext() :
		m_wind(nullptr),
		m_client(nullptr),
		m_eventManager(nullptr),
	
		m_textureManager(nullptr),
		//m_fontManager(nullptr),
		//m_audioManager(nullptr),
		//m_soundManager(nullptr),
		m_systemManager(nullptr),
		m_entityManager(nullptr),
		m_gameMap(nullptr)
		//m_guiManager(nullptr) 
	{}

		Window* m_wind;
		Client* m_client;
		EventManager* m_eventManager;
		ClientSystemManager* m_systemManager;
		ClientEntityManager* m_entityManager;
		Map* m_gameMap;
		TextureManager* m_textureManager;
		/*
		FontManager* m_fontManager;
		AudioManager* m_audioManager;
		SoundManager* m_soundManager;
		
		GUI_Manager* m_guiManager;
		DebugOverlay m_debugOverlay;*/
	
};