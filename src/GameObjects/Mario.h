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
#include "Logger.h"
#include "NetworkGameObject.h"


class Mario : public NetworkGameObject {
public:
    Mario(float x, float y, bool isPlayer = true);

    ~Mario() override {
        EventBus::getInstance().removeSubscribe("onCollision" + this->tag);
        LOG_DEBUG_FMT("The object tagged {} is destroyed", this->getTag());
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

    void destroy() override;

    sf::Vector2f getCenter() override {
        return this->position + getComponent<Collision>()->getOffset() + this->size * 0.5f;
    }

    void serialize(sf::Packet& packet, NetworkMsg type) override;

    void deserialize(sf::Packet& packet) override;

private:
    bool isPlayer = true;
    bool could_shoot = true;
    Timer shoot_timer;
#ifndef SERVER_BUILD
    sf::Sound shoot_sound;
#endif
};
