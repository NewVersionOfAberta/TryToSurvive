#include "C_Shape.h"

C_Shape::C_Shape() : C_Drawable(Component::Shape), m_size(20, 20)
{
}

C_Shape::~C_Shape()
{
}

void C_Shape::UpdatePosition(const sf::Vector2f& l_vec)
{
    m_position = l_vec;
}

const sf::Vector2u& C_Shape::GetSize()
{
    return m_size;
}

void C_Shape::Draw(Window* l_wind)
{
    l_wind->DrawEllipse(m_position, m_size);
}
