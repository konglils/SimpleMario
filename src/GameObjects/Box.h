//
// Created by MINEC on 2026/3/18.
//

#pragma once
#include "Animation.h"
#include "BoxGameObject.h"
#include "FrameManager.h"

class Box : public BoxGameObject {
public:
    Box(const float x, const float y, const std::string& tag = "box")
            : BoxGameObject(x, y, 0, 0) {
        this->tag = tag + ":" + std::to_string(id);

        last_y = y;

        animation.setFrames(FrameManager::getInstance().getFrame("box_frame"));
        animation.setBack(true);

        const auto w = animation.getSprite().getGlobalBounds().width;
        const auto h = animation.getSprite().getGlobalBounds().height;
        this->setSize(w, h);
        this->getComponent<Collision, BoxCollision>()->setSize(w, h);

        this->removeComponent<CollisionHandle>();

        this->addComponent<MoveComponent>();
        this->addComponent<GravityComponent>()->setActive(false);
        className = "Box";
    }

    void start() override {
        GameObject::start();
        EventBus::getInstance().subscribe<CollisionEvent>("onCollision" + this->tag,
            [this](const CollisionEvent& event) {
                const auto& this_pos = event.a_position;
                const auto& other_pos = event.b_position;
                if (other_pos.y < this_pos.y) return;

                // 水平重合量
                const auto w = std::min(this_pos.x + this->getSize().x, other_pos.x + event.b->getSize().x) -
                    std::max(this_pos.x, other_pos.x);

                if (w < 0.4 * this->getSize().x) return;

                const auto& move_component = this->getComponent<MoveComponent>();
                move_component->setSpeedY(-300.f);
                this->getComponent<GravityComponent>()->setActive(true);
            }
        );
    }

    void update(sf::Time deltaTime) override {
        BoxGameObject::update(deltaTime);
        animation.update(deltaTime);
        if (this->getPosition().y > last_y) {
            const auto& move_component = this->getComponent<MoveComponent>();
            move_component->setPositionY(last_y);
            move_component->setSpeedY(0.f);

            this->getComponent<GravityComponent>()->setActive(false);
        }
    }

    void setPosition(const float posX, const float posY) override {
        this->position = sf::Vector2f(posX, posY);
        const auto boxCollision = this->getComponent<Collision, BoxCollision>();
        boxCollision->setPosition(posX, posY);
    }

    void render(sf::RenderWindow* window) override {
        animation.render(window, this->getPosition());
        BoxGameObject::render(window);
    }

private:
    Animation animation;
    float last_y;
};
