#include "SpriteSheet.h"

SpriteSheet::SpriteSheet(TextureManager* l_textMgr)
	:m_textureManager(l_textMgr), m_animationCurrent(nullptr),
	m_spriteScale(1.f, 1.f), m_direction(Direction::Down) {}

SpriteSheet::~SpriteSheet() { ReleaseSheet(); }

void SpriteSheet::ReleaseSheet() {
	m_textureManager->ReleaseResource(m_texture);
	m_animationCurrent = nullptr;
	while (m_animations.begin() != m_animations.end()) {
		delete m_animations.begin()->second;
		m_animations.erase(m_animations.begin());
	}
}

const sf::Vector2u& SpriteSheet::GetSpriteSize()const { return m_spriteSize; }
const sf::Vector2f& SpriteSheet::GetSpritePosition()const {
	return m_sprite.GetPosition();
}
Direction SpriteSheet::GetDirection() { return m_direction; }
Anim_Base* SpriteSheet::GetCurrentAnim() {
	return m_animationCurrent;
}

void SpriteSheet::SetSpriteSize(const sf::Vector2u& l_size) {
	m_spriteSize = l_size;
	m_sprite.SetOrigin(sf::Vector2u(m_spriteSize.first / 2, m_spriteSize.second));
}

void SpriteSheet::SetSpritePosition(const sf::Vector2f& l_pos) {
	m_sprite.SetPosition(l_pos);
}

void SpriteSheet::SetDirection(const Direction& l_dir) {
	if (l_dir == m_direction) { return; }
	m_direction = l_dir;
	m_animationCurrent->CropSprite();
}

void SpriteSheet::SetSheetPadding(const sf::Vector2f& l_padding) {
	m_sheetPadding = l_padding;
}

void SpriteSheet::SetSpriteSpacing(const sf::Vector2f& l_spacing) {
	m_spriteSpacing = l_spacing;
}

const sf::Vector2f& SpriteSheet::GetSheetPadding()const {
	return m_sheetPadding;
}

const sf::Vector2f& SpriteSheet::GetSpriteSpacing()const {
	return m_spriteSpacing;
}

void SpriteSheet::CropSprite(const sf::IntRect& l_rect) {
	m_sprite.SetTextureRect(l_rect);
}

bool SpriteSheet::LoadSheet(const std::string& l_file) {
	std::ifstream sheet;
	sheet.open(Utils::GetWorkingDirectory() + l_file);
	if (!sheet.is_open()) {
		std::cout << "! Failed loading spritesheet: "
			<< l_file << std::endl;
		return false;
	}
	// Release the current spritesheet resources.
	ReleaseSheet();
	std::string line;
	while (std::getline(sheet, line)) {
		if (line[0] == '|') { continue; }
		std::stringstream keystream(line);
		std::string type;
		keystream >> type;
		if (type == "Texture") {
			if (m_texture != "") {
				std::cout << "! Duplicate texture entries in: " << l_file << std::endl;
				continue;
			}
			std::string texture;
			keystream >> texture;
			if (!m_textureManager->RequireResource(texture))
			{
				std::cout << "! Could not set up the texture: " << texture << std::endl;
				continue;
			}
			m_texture = texture;
			m_sprite.SetTexture(m_textureManager->GetResource(m_texture));
		}
		else if (type == "Size") {
			keystream >> m_spriteSize.first >> m_spriteSize.second;
			SetSpriteSize(m_spriteSize);
		}
		else if (type == "Padding") {
			keystream >> m_sheetPadding.first >> m_sheetPadding.second;
		}
		else if (type == "Spacing") {
			keystream >> m_spriteSpacing.first >> m_spriteSpacing.second;
		}
		else if (type == "AnimationType") {
			keystream >> m_animType;
		}
		else if (type == "Animation") {
			std::string name;
			keystream >> name;
			if (m_animations.find(name) != m_animations.end()) {
				std::cerr << "! Duplicate animation(" << name << ") in: " << l_file << std::endl;
				continue;
			}
			Anim_Base* anim = nullptr;
			if (m_animType == "Directional") {
				anim = new Anim_Directional();
			}
			else {
				std::cerr << "! Unknown animation type: " << m_animType << std::endl;
				continue;
			}

			keystream >> *anim;
			anim->SetSpriteSheet(this);
			anim->SetName(name);
			anim->Reset();
			m_animations.emplace(name, anim);

			if (m_animationCurrent) { continue; }
			m_animationCurrent = anim;
			m_animationCurrent->Play();
		}
	}
	sheet.close();
	return true;
}

bool SpriteSheet::SetAnimation(const std::string& l_name, bool l_play, bool l_loop)
{
	auto itr = m_animations.find(l_name);
	if (itr == m_animations.end()) { return false; }
	if (itr->second == m_animationCurrent) { return false; }
	if (m_animationCurrent) {
		m_animationCurrent->Stop();
	}
	m_animationCurrent = itr->second;
	m_animationCurrent->SetLooping(l_loop);
	if (l_play) { m_animationCurrent->Play(); }
	m_animationCurrent->CropSprite();
	return true;
}

void SpriteSheet::Update(float l_dT) {
	m_animationCurrent->Update(l_dT);
}

void SpriteSheet::Draw(Window* l_wnd) {
	l_wnd->Draw(m_sprite);
}