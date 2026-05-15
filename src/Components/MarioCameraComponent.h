//
// Created by MINEC on 2025/12/18.
//

#pragma once
#ifndef SERVER_BUILD
#include "Component.h"
#include "SceneContext.h"
#include "Camera.h"
#include "GameObject.h"

class MarioCameraComponent : public Component {
public:
    MarioCameraComponent() {
        if (const Camera* camera = SceneContext::getInstance().getCamera()) {
            this->position = camera->getPosition();
        }
    }
    void update(const sf::Time& deltaTime) override {
        if (owner->getPosition().x > 500) this->setTargetPositionX(owner->getPosition().x - 500);
        else this->setTargetPositionX(0);
        if (this->target_position != this->position) {
            if (Camera* camera = SceneContext::getInstance().getCamera()) {
                position = position + (target_position - position) * 0.03f;
                camera->setPosition(position.x, position.y);
            }
        }
    }

    void setTargetPosition(const sf::Vector2f& pos) {
        this->target_position = pos;
    }

    void setTargetPositionX(const float x) {
        this->target_position.x = x;
    }

private:
    sf::Vector2f target_position;
    sf::Vector2f position;
};
#endif