//
// Created by MINEC on 2025/12/10.
//

#pragma once

#include "Component.h"
#include "GameObject.h"
#include "Collision.h"

class MoveComponent : public Component {
public:
    MoveComponent() = default;
    void update(const sf::Time& deltaTime) override {
        owner->position += owner->speed * deltaTime.asSeconds();
        setPosition(owner->position.x, owner->position.y);
    }

    void setPosition(const sf::Vector2f& pos, const bool move_collision = true) const {
        owner->setPosition(pos.x, pos.y);
        if (move_collision) setCollisionPosition(pos);
    }

    void moveCollisionTo(const sf::Vector2f& pos) const {
        if (const auto& collision = owner->getComponent<Collision>()) {
            const auto delta_pos = pos - collision->getCollisionPosition();
            collision->setCollisionPosition(pos);
            this->addPosition(delta_pos, false);
        }
    }

    void moveCollisionXTo(const float posX) const {
        const auto& collision = owner->getComponent<Collision>();
        moveCollisionTo(sf::Vector2f(posX, collision->getCollisionPosition().y));
    }

    void moveCollisionYTo(const float posY) const {
        const auto& collision = owner->getComponent<Collision>();
        moveCollisionTo(sf::Vector2f(collision->getCollisionPosition().x, posY));
    }

    void setPosition(const float posX, const float posY, const bool move_collision = true) const {
        setPosition(sf::Vector2f(posX, posY), move_collision);
    }

    void setPositionX(const float posX, const bool move_collision = true) const {
        setPosition(sf::Vector2f(posX, owner->position.y), move_collision);
    }

    void setPositionY(const float posY, const bool move_collision = true) const {
        setPosition(sf::Vector2f(owner->position.x, posY), move_collision);
    }

    void addPosition(const sf::Vector2f& pos, const bool move_collision = true) const {
        setPosition(owner->position + pos, move_collision);
    }

    void setSpeed(const sf::Vector2f& speed) const {
        owner->speed = speed;
    }

    void setSpeedX(const float speedX) const {
        owner->speed.x = speedX;
    }

    void setSpeedY(const float speedY) const {
        owner->speed.y = speedY;
    }

    void setSpeed(const float speedX, const float speedY) const {
        owner->speed = sf::Vector2f(speedX, speedY);
    }

    void addSpeed(const sf::Vector2f& speed) const {
        owner->speed += speed;
    }

private:
    void setCollisionPosition(const sf::Vector2f& pos) const {
        if (const auto& collision = owner->getComponent<Collision>()) {
            collision->setPosition(pos);
        }
    }
};


