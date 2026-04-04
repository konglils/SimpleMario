//
// Created by MINEC on 2026/2/2.
//

#pragma once

#include "Component.h"
#include <SFML/Graphics.hpp>
#include "GameObject.h"
#include "MoveComponent.h"
#include "StateMachine.h"

class MarioController : public Component {
public:
    void handleEvent(const sf::Event& event) override {
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::A) {
                runLeft();
            }
            if (event.key.code == sf::Keyboard::D) {
                runRight();
            }
            if (event.key.code == sf::Keyboard::W) {
                jump();
            }
        } else if (event.type == sf::Event::KeyReleased) {
            if (event.key.code == sf::Keyboard::A) {
                stopRun();
            }
            if (event.key.code == sf::Keyboard::D) {
                stopRun();
            }
        }
    }

    void jump() const {
        std::shared_ptr<MoveComponent> moveComponent = owner->getComponent<MoveComponent>();
        if (!moveComponent) {
            moveComponent = owner->addComponent<MoveComponent>();
        }
        auto state = owner->getComponent<StateMachine>();
        if (state && state->getCurrentStateName() != "MarioJumpState")
            moveComponent->setSpeedY(-ConfigManager::getInstance().game.jumpForce);
    }

    void runLeft() const {
        std::shared_ptr<MoveComponent> moveComponent = owner->getComponent<MoveComponent>();
        if (!moveComponent) {
            moveComponent = owner->addComponent<MoveComponent>();
        }
        moveComponent->setSpeedX(-ConfigManager::getInstance().game.playerSpeed);
    }

    void runRight() const {
        std::shared_ptr<MoveComponent> moveComponent = owner->getComponent<MoveComponent>();
        if (!moveComponent) {
            moveComponent = owner->addComponent<MoveComponent>();
        }
        moveComponent->setSpeedX(ConfigManager::getInstance().game.playerSpeed);
    }

    void stopRun() const {
        std::shared_ptr<MoveComponent> moveComponent = owner->getComponent<MoveComponent>();
        if (!moveComponent) {
            moveComponent = owner->addComponent<MoveComponent>();
        }
        moveComponent->setSpeedX(0.f);
    }

};

