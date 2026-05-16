//
// Created by MINEC on 2026/1/30.
//

#pragma once

#include "AssetManager.h"
#include "BaseState.h"
#include "GameObject.h"
#include "StateMachine.h"


class MarioIdleState : public BaseState {
public:
    explicit MarioIdleState() : BaseState("MarioIdleState") {
#ifndef SERVER_BUILD
        const sf::Texture& mario_texture = AssetManager::getInstance().getTexture("mario_bros");
        right_sprite.setTexture(mario_texture);
        right_sprite.setTextureRect(sf::IntRect(178, 32, 12, 16));
        right_sprite.setScale(4.f, 4.f);
        left_sprite.setTexture(mario_texture);
        left_sprite.setTextureRect(sf::IntRect(178, 32, 12, 16));
        left_sprite.setScale(-4.f, 4.f);
        left_sprite.setOrigin(static_cast<float>(right_sprite.getTextureRect().width), 0.f);
        jump_sound.setBuffer(AssetManager::getInstance().getSoundBuffer("small_jump"));
#endif
    }
    ~MarioIdleState() override = default;

    void start() override {
        const auto box_collision = owner->getComponent<Collision, BoxCollision>();
#ifndef SERVER_BUILD
        const float w = left_sprite.getGlobalBounds().width;
        const float h = left_sprite.getGlobalBounds().height;
        LOG_TRACE_FMT("MarioIdle sprite width:{}, height:{}", w, h);
#else
        const float w = 48.f;
        const float h = 64.f;
#endif
        box_collision->setSize(w, h);
        owner->setSize(w, h);
        box_collision->setOffset(sf::Vector2f(0.f, 0.f));
    }

    void update(const sf::Time& deltaTime) override {
        if (owner->getSpeed().x != 0.f) {
            owner->getComponent<StateMachine>()->setState("MarioRunState");
        }
    }

    void handleEvent(const sf::Event& event) override {
        // 防止错误更新
        if (owner->getComponent<StateMachine>()->getCurrentStateName() != this->getName()) return;
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::A) {
                setIsLeft(true);
            } else if (event.key.code == sf::Keyboard::D) {
                setIsLeft(false);
            } else if (event.key.code == sf::Keyboard::W) {
#ifndef SERVER_BUILD
                jump_sound.stop();
                jump_sound.play();
#endif
                owner->getComponent<StateMachine>()->setState("MarioJumpState");
                std::dynamic_pointer_cast<MarioJumpState>(owner->getComponent<StateMachine>()->getCurrentState())->setJumpTimer();
            }
        }
    }
#ifndef SERVER_BUILD
    void render(sf::RenderWindow* window) override {
        if (getIsLeft()) {
            if (owner) left_sprite.setPosition(owner->getPosition());
            else LOG_ERROR("Owner is nullptr");
            window->draw(left_sprite);
        } else {
            if (owner) right_sprite.setPosition(owner->getPosition());
            else LOG_ERROR("Owner is nullptr");
            window->draw(right_sprite);
        }
    }
#endif
    bool getIsLeft() const {
        return owner->getComponent<StateMachine>()->getIsLeft();
    }

    void setIsLeft(const bool value) const {
        owner->getComponent<StateMachine>()->setIsLeft(value);
    }

private:
#ifndef SERVER_BUILD
    sf::Sprite left_sprite;
    sf::Sprite right_sprite;
    sf::Sound jump_sound;
#endif
};
