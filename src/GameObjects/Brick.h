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
#ifndef SERVER_BUILD
        sprite.setTexture(AssetManager::getInstance().getTexture("tile_set"));
        sprite.setTextureRect(sf::IntRect(16, 0, 16, 16));
        sprite.setScale(4.f, 4.f);
        sprite.setPosition(this->getPosition());

        this->setSize(sprite.getGlobalBounds().width, sprite.getGlobalBounds().height);
#else
        this->setSize(CONFIG.game.defaultBlockSize, CONFIG.game.defaultBlockSize);
#endif

#ifndef SERVER_BUILD
        this->getComponent<Collision, BoxCollision>()->setSize(sprite.getGlobalBounds().width, sprite.getGlobalBounds().height);
#else
        this->getComponent<Collision, BoxCollision>()->setSize(CONFIG.game.defaultBlockSize, CONFIG.game.defaultBlockSize);
#endif
        className = "Brick";
    }

    void setPosition(const float posX, const float posY) override {
        this->position = sf::Vector2f(posX, posY);
        const auto boxCollision = this->getComponent<Collision, BoxCollision>();
        boxCollision->setPosition(posX, posY);
    }
#ifndef SERVER_BUILD
    void render(sf::RenderWindow* window) override {
        BoxGameObject::render(window);
        window->draw(sprite);
    }
#endif
private:
#ifndef SERVER_BUILD
    sf::Sprite sprite;
#endif
};