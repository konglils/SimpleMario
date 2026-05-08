//
// Created by MINEC on 2026/3/18.
//

#pragma once
#include "Animation.h"
#include "BoxGameObject.h"

class Box : public BoxGameObject {
public:
    Box(float x, float y, const std::string& tag = "box");

    ~Box() override;

    void start() override;

    void update(sf::Time deltaTime) override;

    void setPosition(float posX, float posY) override;

    void render(sf::RenderWindow* window) override;

private:
    Animation animation;
    float last_y;
};
