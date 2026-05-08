//
// Created by MINEC on 2025/12/18.
//

#pragma once
#include "Component.h"
#include "SceneContext.h"
#include "Camera.h"
#include "GameObject.h"

class CameraComponent : public Component {
public:
    CameraComponent() = default;
    void update(const sf::Time& deltaTime) override {
        if (Camera* camera = SceneContext::getInstance().getCamera()) {
            camera->setPosition(owner->getPosition().x - 400, owner->getPosition().y - 600);
        }
    }
};
