#pragma once
#include "C_Drawable.h"

class C_Shape : public C_Drawable {
	// Унаследовано через C_Drawable
public:
	C_Shape();
	~C_Shape();
	void UpdatePosition(const sf::Vector2f& l_vec);
	const sf::Vector2u& GetSize();
	void Draw(Window* l_wind);
	void ReadIn(std::stringstream& l_stream) { l_stream >> m_position.first >> m_position.second >> m_size.first >> m_size.second; };
	void SetSize(sf::Vector2u l_size) { m_size = l_size; }
private:
	sf::Vector2f m_position;
	sf::Vector2u m_size;
};