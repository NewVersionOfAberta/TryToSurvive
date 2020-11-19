#pragma once
#include "Usings.h"

class View
{
public:

    View();

    View(const sf::Vector2f& center, const sf::Vector2f& size);

    void setCenter(float x, float y);

    void setCenter(const sf::Vector2f& center);

    void setSize(float width, float height);

    void setSize(const sf::Vector2f& size);

    void reset(const sf::Vector2f& top, const sf::Vector2f& size);

    const sf::Vector2f& getCenter() const;

    const sf::Vector2f& getSize() const;


    void move(float offsetX, float offsetY);

    void move(const sf::Vector2f& offset);



private:

    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    sf::Vector2f          m_center;              //!< Center of the view, in scene coordinates
    sf::Vector2f          m_size;                //!< Size of the view, in scene coordinates
};

