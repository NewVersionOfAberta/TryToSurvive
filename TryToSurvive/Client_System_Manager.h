#pragma once
#include "System_Manager.h"
#include "Window.h"
#include "S_Renderer.h"
#include "S_Movement.h"
#include "TextureManager.h"
#include "S_Control.h"
//#include "S_Collision.h"
#include "S_State.h"
//#include "S_Sound.h"
#include "S_SheetAnimation.h"
//#include "S_Sound.h"
//#include "S_Network.h"
//#include "S_CharacterUI.h"
//#include "FontManager.h"

class ClientSystemManager : public SystemManager {
public:
	ClientSystemManager(TextureManager* l_textureMgr/*, FontManager* l_fontMgr*/);
	~ClientSystemManager();

	TextureManager* GetTextureManager();
	//FontManager* GetFontManager();

	void Draw(Window* l_wind, unsigned int l_elevation);
	// Debug.
private:
	TextureManager* m_textureMgr;
	//FontManager* m_fontMgr;
};

