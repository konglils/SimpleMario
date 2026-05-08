//
// Created by MINEC on 2026/1/29.
//

#pragma once
#include "GameObject.h"
#include <SFML/Graphics.hpp>

#include "MarioJumpState.h"
#include "Collision.h"
#include "FireBall.h"
#include "EventBus.h"


class Mario : public GameObject {
public:
    Mario(const float x, const float y, const bool isPlayer = true);

    ~Mario() override {
        EventBus::getInstance().removeSubscribe("onCollision" + this->tag);
    }

    void start() override;

    void handleEvent(sf::Event& e) override;

    void shoot();

    void update(sf::Time deltaTime) override;

    bool needGravity();

    void handleCollision(const CollisionEvent& event);

    bool getIsPlayer() const {
        return isPlayer;
    }

    sf::Vector2f getCenter() override {
        return this->position + getComponent<Collision>()->getOffset() + this->size * 0.5f;
    }

private:
    bool isPlayer = true;
    bool could_shoot = true;
    Timer shoot_timer;
};
