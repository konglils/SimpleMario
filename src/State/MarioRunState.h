//
// Created by MINEC on 2026/1/29.
//

#pragma once

#include "Animation.h"
#include "FrameManager.h"
#include "BaseState.h"
#include "GameObject.h"
#include "StateMachine.h"

class MarioRunState : public BaseState {
public:
    explicit MarioRunState() : BaseState("MarioRunState") {
#ifndef SERVER_BUILD
        animation_right.setFrames(FrameManager::getInstance().getFrame("right_small_normal"));
        animation_left.setFrames(FrameManager::getInstance().getFrame("left_small_normal"));
        jump_sound.setBuffer(AssetManager::getInstance().getSoundBuffer("small_jump"));
#endif
    }
    ~MarioRunState() override = default;


    void update(const sf::Time& deltaTime) override {
        if (owner->getSpeed().x == 0.f) {
            owner->getComponent<StateMachine>()->setState("MarioIdleState");
            return;
        }

        const float speedX = owner->getSpeed().x;
        if (speedX > 0.f) {
            setIsLeft(false);
        } else if (speedX < 0.f) {
            setIsLeft(true);
        }
#ifndef SERVER_BUILD
        this->getAnimation().update(deltaTime);
#endif
        // sf::Sprite* sprite;
        // if (getIsLeft()) {
        //     sprite = &animation_left.getSprite();
        // } else {
        //     sprite = &animation_right.getSprite();
        // }
        // const auto box_collision = owner->getComponent<Collision, BoxCollision>();
        // const float w = sprite->getGlobalBounds().width;
        // const float h = sprite->getGlobalBounds().height;
        // box_collision->setSize(w, h);
        // owner->setSize(w, h);

        const auto box_collision = owner->getComponent<Collision, BoxCollision>();
        if (!getIsLeft()) {
            box_collision->setOffset(sf::Vector2f(12.f, 0.f));
        } else {
            box_collision->setOffset(sf::Vector2f(0.f, 0.f));
        }
    }
    void handleEvent(const sf::Event& event) override {
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::A) {
                setIsLeft(true);
            } else if (event.key.code == sf::Keyboard::D) {
                setIsLeft(false);
            } else if (event.key.code == sf::Keyboard::W) {
#ifndef SERVER_BUILD
                jump_sound.play();
#endif
                owner->getComponent<StateMachine>()->setState("MarioJumpState");
                std::dynamic_pointer_cast<MarioJumpState>(owner->getComponent<StateMachine>()->getCurrentState())->setJumpTimer();
            }
        }
    }
#ifndef SERVER_BUILD
    void render(sf::RenderWindow* window) override {
        if (owner) this->getAnimation().render(window, owner->getPosition());
        else LOG_ERROR("owner is nullptr");
    }
#endif

    bool getIsLeft() const {
        return owner->getComponent<StateMachine>()->getIsLeft();
    }

    void setIsLeft(const bool value) const {
        owner->getComponent<StateMachine>()->setIsLeft(value);
    }
#ifndef SERVER_BUILD
    Animation& getAnimation() {
        if (getIsLeft()) return animation_left;
        return animation_right;
    }
#endif
private:
#ifndef SERVER_BUILD
    Animation animation_right;
    Animation animation_left;
    sf::Sound jump_sound;
#endif
};
