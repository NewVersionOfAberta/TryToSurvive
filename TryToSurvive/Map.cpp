#include "Map.h"
#include "StateManager.h"
#include <C_Position.h>

Map::Map(SharedContext* l_context)
	:m_context(l_context), m_defaultTile(l_context), m_maxMapSize(100, 100), m_playerId(-1)
{
	m_context->m_gameMap = this;
	LoadTiles("media/tile_cfg.txt");
	LoadBackground();
}

Map::~Map() {
	PurgeMap();
	PurgeTileSet();
	m_context->m_gameMap = nullptr;
}

Tile* Map::GetTile(unsigned int l_x, unsigned int l_y, unsigned int l_layer)
{
	if (l_x < 0 || l_y < 0 || l_x >= m_maxMapSize.first || l_y >= m_maxMapSize.second || l_layer < 0 || l_layer >= Sheet::Num_Layers)
	{
		return nullptr;
	}

	auto itr = m_tileMap.find(ConvertCoords(l_x, l_y, l_layer));
	if (itr == m_tileMap.end()) { return nullptr; }
	return itr->second;
}

TileInfo* Map::GetDefaultTile() { return &m_defaultTile; }
unsigned int Map::GetTileSize()const { return Sheet::Tile_Size; }
const sf::Vector2u& Map::GetMapSize()const { return m_maxMapSize; }
const sf::Vector2f& Map::GetPlayerStart()const { return m_playerStart; }
int Map::GetPlayerId()const { return m_playerId; }

void Map::LoadMap(const std::string& l_path) {
	std::ifstream mapFile;
	mapFile.open(Utils::GetWorkingDirectory() + l_path);
	if (!mapFile.is_open()) {
		std::cout << "! Failed loading map file: " << l_path << std::endl;
		return;
	}
	std::string line;
	int tileId = 0;
	int curLayer = 0;
	std::cout << "--- Loading a map: " << l_path << std::endl;
	while (std::getline(mapFile, line)) {
		if (line[0] == '|') { continue; }
		std::stringstream keystream(line);
		std::string type;
		keystream >> type;
		if (type == "LAYER") {
			keystream >> curLayer;
			if (curLayer < 0) {
				std::cout << "! Bad layer: " << curLayer << std::endl;
				continue;
			}
			for (int j = 0; j < m_maxMapSize.first; j++) {
				std::getline(mapFile, line);
				std::stringstream tilestream(line);
				for (int i = 0; i < m_maxMapSize.second; i++) {
					tilestream >> tileId;
					--tileId;
					if (tileId < 0) { continue; }
					auto itr = m_tileSet.find(tileId);
					if (itr == m_tileSet.end()) {
						std::cout << "! Tile id(" << tileId << ") was not found in tileset." << std::endl;
						continue;
					}
					Tile* tile = new Tile();
					// Bind properties of a tile from a set.
					tile->m_properties = itr->second;
					if (!m_tileMap.emplace(ConvertCoords(
						i, j, curLayer), tile).second)
					{
						// Duplicate tile detected!
						std::cout << "! Duplicate tile! : " << i << " " << j << std::endl;
						delete tile;
						continue;
					}

				}
			}
		}
		else if (type == "SIZE") {
			keystream >> m_maxMapSize.first >> m_maxMapSize.second;
		}
		else if (type == "DEFAULT_FRICTION") {
			keystream >> m_defaultTile.m_friction;
		}
		else if (type == "ENTITY") {
			// Set up entity here.
			std::string name;
			keystream >> name;
			// Redo
			if (name == "Player" && m_playerId != -1) { continue; }
			int entityId = m_context->m_entityManager->AddEntity(name);
			if (entityId < 0) { continue; }
			if (name == "Player") { m_playerId = entityId; }
			C_Base* position = m_context->m_entityManager->
				GetComponent<C_Position>(entityId, Component::Position);
			if (position) { keystream >> *position; }
		}
		else {
			// Something else.
			std::cout << "! Unknown type \"" << type << "\"." << std::endl;
		}
	}
	mapFile.close();
	std::cout << "--- Map Loaded! ---" << std::endl;
}

void Map::LoadTiles(const std::string& l_path) {
	std::ifstream tileSetFile;
	tileSetFile.open(Utils::GetWorkingDirectory() + l_path);
	if (!tileSetFile.is_open()) {
		std::cout << "! Failed loading tile set file: " << l_path << std::endl;
		return;
	}
	std::string line;
	while (std::getline(tileSetFile, line)) {
		if (line[0] == '|') { continue; }
		std::stringstream keystream(line);
		int tileId;
		keystream >> tileId;
		if (tileId < 0) { continue; }
		TileInfo* tile = new TileInfo(m_context, "TileSheet", tileId);
		keystream >> tile->m_friction >> tile->m_deadly >> tile->m_solid;
		if (!m_tileSet.emplace(tileId, tile).second) {
			// Duplicate tile detected!
			std::cout << "! Duplicate tile type: " << tile->m_id << std::endl;
			delete tile;
			tile = nullptr;
		}
	}
	tileSetFile.close();
}

void Map::LoadBackground()
{
	if (!m_context->m_textureManager->RequireResource("Background")) 
	{ 
		std::cout << "Failed to loat background";
		return; 
	}
	m_background = m_context->m_textureManager->GetResource("Background");
}

void Map::DrawBackground(sf::FloatRect viewSpace)
{
	m_context->m_wind->Draw(viewSpace.left, viewSpace.top,
		viewSpace.width, viewSpace.height,
		0, 0, m_background);
}


void Map::Update(float l_dT) {}

void Map::Draw(unsigned int l_layer) {
	if (l_layer >= Sheet::Num_Layers) { return; }
	//sf::RenderWindow* l_wind = m_context->m_wind->GetRenderWindow();
	sf::FloatRect viewSpace = m_context->m_wind->GetViewSpace();
	sf::Vector2i tileBegin(floor(viewSpace.left / Sheet::Tile_Size), floor(viewSpace.top / Sheet::Tile_Size));
	sf::Vector2i tileEnd(ceil((viewSpace.left + viewSpace.width) / Sheet::Tile_Size),
		ceil((viewSpace.top + viewSpace.height) / Sheet::Tile_Size));

	unsigned int count = 0;
	for (int x = tileBegin.first - 1; x <= tileEnd.first + 1; ++x) {
		for (int y = tileBegin.second - 1; y <= tileEnd.second + 1; ++y) {
			Tile* tile = GetTile(x, y, l_layer);
			if (!tile) { continue; }
			m_context->m_wind->Draw(
				x * Sheet::Tile_Size,
				y * Sheet::Tile_Size,
				Sheet::Tile_Size,
				Sheet::Tile_Size,
				tile->m_properties->m_x,
				tile->m_properties->m_y,
				tile->m_properties->m_bmp);
			++count;

		}
	}
}

unsigned int Map::ConvertCoords(unsigned int l_x, unsigned int l_y, unsigned int l_layer)const
{
	return ((l_layer * m_maxMapSize.second + l_y) * m_maxMapSize.first + l_x);
}

void Map::PurgeMap() {
	while (m_tileMap.begin() != m_tileMap.end()) {
		delete m_tileMap.begin()->second;
		m_tileMap.erase(m_tileMap.begin());
	}
	m_tileCount = 0;
	m_context->m_entityManager->Purge();
}

void Map::PurgeTileSet() {
	while (m_tileSet.begin() != m_tileSet.end()) {
		delete m_tileSet.begin()->second;
		m_tileSet.erase(m_tileSet.begin());
	}
	m_tileSetCount = 0;
}