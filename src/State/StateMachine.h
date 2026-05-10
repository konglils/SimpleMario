//
// Created by MINEC on 2026/1/29.
//

#pragma once
#include <unordered_map>
#include "BaseState.h"

class StateMachine : public Component {
public:
    StateMachine() = default;
    explicit StateMachine(GameObject* owner) : Component(owner) {}
    ~StateMachine() override = default;

    void update(const sf::Time& deltaTime) override {
        if (currentState) {
            currentState->update(deltaTime);
        }
        // std::cout << "current state: " << currentState->getName() << std::endl;
    }

    void render(sf::RenderWindow* window) override {
        if (currentState) {
            currentState->render(window);
        }
    }

    void handleEvent(const sf::Event& event) override {
        if (currentState) {
            currentState->handleEvent(event);
        }
    }

    template<typename T, typename... Args>
    void addState(Args&&... args) {
        std::shared_ptr<T> temp = std::make_shared<T>(std::forward<Args>(args)...);
        temp->setOwner(owner);
        states[temp->getName()] = temp;
    }

    void setState(const std::string& stateName) {
        if (!states.contains(stateName)) {
            LOG_INFO_FMT("State {} does not exist!", stateName);
            return;
        }
        if (currentState) {
            currentState->stop();
        }
        currentState = states[stateName];
        currentState->start();
    }

    bool getIsLeft() const {
        return isLeft;
    }

    void setIsLeft(const bool value) {
        this->isLeft = value;
    }

    std::string getCurrentStateName() const {
        if (currentState) {
            return currentState->getName();
        }
        return "null";
    }

    const std::shared_ptr<BaseState>& getCurrentState() const {
        return currentState;
    }

private:
    std::shared_ptr<BaseState> currentState{};
    std::unordered_map<std::string, std::shared_ptr<BaseState>> states;
    bool isLeft = false;
};
