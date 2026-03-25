//
// Created by MINEC on 2026/1/4.
//

#pragma once

#include "GameObject3D.h"
#include "ModelManager.h"

class Human3D : public GameObject3D {
public:
    Human3D() {
        Model* p = ModelManager::getInstance().getModel("human");
        if (p == nullptr) {
            ModelManager::getInstance().loadModel("./Asset/human.obj", "human");
            this->model = ModelManager::getInstance().getModel("human");
        } else {
            this->model = p;
        }
        position = {0.0f, -3.0f, 5.0f};
        className = "Human3D";
    }
    void render(sf::RenderWindow* window) override {
        // drawPoints(window);
        drawFaces(window);
    }

    void update(const sf::Time deltaTime) override {
        angleXZ += deltaTime.asSeconds();
    }
};