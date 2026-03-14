//
// Created by MINEC on 2026/3/14.
//

#pragma once

#include "AssetManager.h"
#include "BoxGameObject.h"

class Brick : public BoxGameObject {
public:
    Brick(const float x, const float y, const std::string& tag = "brick")
            : BoxGameObject(x, y, 0, 0) {
        this->tag = tag + ":" + std::to_string(id);
        this->moveAble = false;

        sprite.setTexture(AssetManager::getInstance().getTexture("tile_set"));
        sprite.setTextureRect(sf::IntRect(16, 0, 15, 15));
        sprite.setScale(4.f, 4.f);
        sprite.setPosition(this->getPosition());

        this->setSize(sprite.getGlobalBounds().width, sprite.getGlobalBounds().height);
        this->getComponent<Collision, BoxCollision>()->setSize(sprite.getGlobalBounds().width, sprite.getGlobalBounds().height);
        className = "Brick";
    }

    void setPosition(const float posX, const float posY) override {
        this->position = sf::Vector2f(posX, posY);
        const auto boxCollision = this->getComponent<Collision, BoxCollision>();
        boxCollision->setPosition(posX, posY);
    }

    void render(sf::RenderWindow* window) override {
        BoxGameObject::render(window);
        window->draw(sprite);
    }

private:
    sf::Sprite sprite;
};