//
// Created by MINEC on 2025/12/9.
//

#pragma once

#include "Component.h"
#include "GameObject.h"
#include "SceneContext.h"
#include "MoveComponent.h"
#include "ConfigManager.h"

class GravityComponent : public Component {
public:
    GravityComponent() = default;
    void update(const sf::Time& deltaTime) override {
        float worldHeight = SceneContext::getInstance().getWindowHeight();

        if (std::abs(this->owner->getPosition().y + this->owner->getSize().y - worldHeight) < 0.1f
            && std::abs(owner->getSpeed().y) <= 1.f) return;

        std::shared_ptr<MoveComponent> moveComponent = owner->getComponent<MoveComponent>();
        if (!moveComponent) return;
        moveComponent->setSpeedY(owner->getSpeed().y + gravity * deltaTime.asSeconds());

    }
    std::string getName() {
        return name;
    }
private:
    float gravity = ConfigManager::getInstance().game.gravity;
    std::string name = "GravityComponent";
};


