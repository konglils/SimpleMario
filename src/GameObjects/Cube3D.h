//
// Created by MINEC on 2026/1/3.
//

#pragma once

#include "GameObject3D.h"
#include "ModelManager.h"

class Cube3D : public GameObject3D {
public:
    Cube3D() {
        Model* p = ModelManager::getInstance().getModel("cube");
        if (p == nullptr) {
            ModelManager::getInstance().loadModel(ConfigManager::getInstance().getModelPath("cube"), "cube");
            this->model = ModelManager::getInstance().getModel("cube");
        } else {
            this->model = p;
        }
        position = {0.0f, 0.0f, 3.0f};
        className = "Cube3D";
    }
};