#pragma once
#include <vector>
#include "C_Movable.h"
#include "C_State.h"
#include "Entity_Manager.h"
#include "C_Position.h"
#include "TextureManager.h"
#include "C_SpriteSheet.h"
#include "C_Collidable.h"
#include "C_Shape.h"
#include "C_Client.h"



class SystemManager;

class ClientEntityManager : public EntityManager {
public:
	ClientEntityManager(SystemManager* l_sysMgr, TextureManager* l_textureMgr);
	~ClientEntityManager();

	int AddEntity(const std::string& l_entityFile, int l_id = -1);
protected:
	TextureManager* m_textureManager;
};