//
// Created by MINEC on 2026/3/25.
//

#pragma once

#include "Cube3D.h"

class Cube3DWithController : public Cube3D {
public:
    Cube3DWithController() {
        className = "Cube3DWithController";
        SceneContext::getInstance().getCamera()->setMouseControl(false);
    }

    void update(const sf::Time deltaTime) override {

    }

    void handleEvent(sf::Event& event) override {
        if (event.type == sf::Event::MouseButtonPressed) {
            mouse_is_pressed = true;
            mousePos = sf::Mouse::getPosition(*SceneContext::getInstance().getWindow());
        } else if (event.type == sf::Event::MouseButtonReleased) {
            mouse_is_pressed = false;
            for (auto& point : model->points) {
                point = rotate(point);
            }
            angleXZ = 0.0f;
            angleYZ = 0.0f;
        } else if (event.type == sf::Event::MouseMoved) {
            const sf::Vector2i pos = sf::Mouse::getPosition(*SceneContext::getInstance().getWindow());
            if (mouse_is_pressed) {
                const sf::Vector2i delta = pos - mousePos;
                angleYZ -= delta.y * 0.01f;
                angleXZ += delta.x * 0.01f;
                mousePos = pos;
            }
        }
    }

private:
    bool mouse_is_pressed{};
    sf::Vector2i mousePos;
};