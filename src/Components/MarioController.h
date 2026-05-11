//
// Created by MINEC on 2026/2/2.
//

#pragma once

#include "Component.h"

class MarioController : public Component {
public:
    void handleEvent(const sf::Event& event) override;

    void jump() const;

    void runLeft() const;

    void runRight() const;

    void stopRun() const;
};

