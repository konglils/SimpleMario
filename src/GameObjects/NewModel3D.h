//
// Created by MINEC on 2026/1/4.
//

#pragma once

#include "GameObject.h"
#include "ModelManager.h"

class NewModel3D : public GameObject3D {
public:
    NewModel3D() {
        Model* p = ModelManager::getInstance().getModel("new_model");
        if (p == nullptr) {
            ModelManager::getInstance().loadModel("./Asset/new_model.obj", "new_model");
            this->model = ModelManager::getInstance().getModel("new_model");
        } else {
            this->model = p;
        }
        position = {0.f, -0.6f, 1.f};
        className = "NewModel3D";
    }
    void render(sf::RenderWindow* window) override {
        // drawPoints(window);
        drawFaces(window);
    }

    void update(const sf::Time deltaTime) override {
        angleXZ += deltaTime.asSeconds();
    }
};