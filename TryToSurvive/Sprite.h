#pragma once
#include "Usings.h"

class Sprite {
public:
	Sprite() :
		m_texture(NULL),
		m_textureRect(0, 0, 0, 0)
	{
	}
	void SetOrigin(sf::Vector2u l_origin) { m_origin = l_origin; }
	void SetPosition(sf::Vector2f l_position) { m_position = l_position; }
	sf::Vector2f GetPosition() const { return m_position; }
	void SetTextureRect(sf::IntRect l_rect) { m_textureRect = l_rect; }
	sf::IntRect GetTextureRect() { return m_textureRect; }
	HBITMAP GetTexture() { return m_texture; }
	void SetTexture(HBITMAP l_texture) { m_texture = l_texture; }
private:
	sf::Vector2u m_origin;
	sf::Vector2f m_position;
	sf::IntRect m_textureRect;
	HBITMAP m_texture;
};