#include "View.h"

View::View() :
    m_center(),
    m_size()
{
    reset(sf::Vector2f(0, 0), sf::Vector2f(1000, 1000));
}

View::View(const sf::Vector2f& center, const sf::Vector2f& size) :
    m_center(center),
    m_size(size)
{

}

void View::setCenter(float x, float y)
{
    m_center.first = x;
    m_center.second = y;
}


void View::setCenter(const sf::Vector2f& center)
{
    setCenter(center.first, center.second);
}


void View::setSize(float width, float height)
{
    m_size.first = width;
    m_size.second = height;
}


void View::setSize(const sf::Vector2f& size)
{
    setSize(size.first, size.second);
}


void View::reset(const sf::Vector2f& top, const sf::Vector2f& size)
{
    m_center.first = top.first + size.first / 2.f;
    m_center.second = top.second + size.second / 2.f;
    m_size = size;
}


const sf::Vector2f& View::getCenter() const
{
    return m_center;
}


const sf::Vector2f& View::getSize() const
{
    return m_size;
}


void View::move(float offsetX, float offsetY)
{
    setCenter(m_center.first + offsetX, m_center.second + offsetY);
}


void View::move(const sf::Vector2f& offset)
{  
    sf::Vector2f l_center = m_center;
    l_center.first += offset.first;
    l_center.second += offset.second;
    setCenter(l_center);
}

