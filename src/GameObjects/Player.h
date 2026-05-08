//
// Created by MINEC on 2025/12/9.
//


#pragma once
#include <SFML/Graphics.hpp>
#include "GameObject.h"
#include "EventBus.h"
#include <string>


class Player : public GameObject {
public:
    Player(float x, float y, float radius, const std::string& tag = "player");

    ~Player() override {
        EventBus::getInstance().removeSubscribe("onCollision" + this->tag);
    }

    void render(sf::RenderWindow* window) override {
        window->draw(shape);
        renderComponents(window);
    }

    void start() override;

private:
    void setPosition(const float x, const float y) override {
        // std::cout << "Player setPosition:" << x << " " << y << std::endl;
        GameObject::setPosition(x, y);
        shape.setPosition(x, y);
    }

    void setSpeed(const float x, const float y) {
        this->speed.x = x;
        this->speed.y = y;
    }

    sf::CircleShape shape;
};


