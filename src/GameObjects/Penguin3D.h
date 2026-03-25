//
// Created by MINEC on 2026/1/3.
//

#pragma once

#include "GameObject3D.h"

class Penguin3D : public GameObject3D {
public:
    Penguin3D() {
        Model* p = ModelManager::getInstance().getModel("penguin");
        if (p == nullptr) {
            ModelManager::getInstance().loadModel("./Asset/penguin.obj", "penguin");
            this->model = ModelManager::getInstance().getModel("penguin");
        } else {
            this->model = p;
        }
        position = {0.0f, -0.5f, 1.0f};
        className = "Penguin3D";
    }

    void render(sf::RenderWindow* window) override {
        // drawPoints(window);
        drawFaces(window);
    }

    void update(const sf::Time deltaTime) override {
        angleXZ += deltaTime.asSeconds();
    }
};