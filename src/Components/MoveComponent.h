//
// Created by MINEC on 2025/12/10.
//

#pragma once

#include "Component.h"

class MoveComponent : public Component {
public:
    MoveComponent() = default;
    void update(const sf::Time& deltaTime) override;

    void render(sf::RenderWindow* window) override;

    void setPosition(const sf::Vector2f& pos, bool move_collision = true) const;

    void moveCollisionTo(const sf::Vector2f& pos) const;

    void moveCollisionXTo(float posX) const;

    void moveCollisionYTo(float posY) const;

    void setPosition(float posX, float posY, bool move_collision = true) const;

    void setPositionX(float posX, bool move_collision = true) const;

    void setPositionY(float posY, bool move_collision = true) const;

    void addPosition(const sf::Vector2f& pos, bool move_collision = true) const;

    void setSpeed(const sf::Vector2f& speed) const;

    void setSpeedX(float speedX) const;

    void setSpeedY(float speedY) const;

    void setSpeed(float speedX, float speedY) const;

    void addSpeed(const sf::Vector2f& speed) const;

    static void drawArrow(sf::RenderWindow* window, float x1, float y1, float x2, float y2,
                          float arrowSize = 10.0f, sf::Color color = sf::Color::Red);

private:
    void setCollisionPosition(const sf::Vector2f& pos) const;
};


