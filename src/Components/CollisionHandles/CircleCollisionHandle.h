//
// Created by MINEC on 2025/12/11.
//

#pragma once

#include "CollisionHandle.h"

class CircleCollisionHandle : public CollisionHandle {
public:
    CircleCollisionHandle();

    void handleCollisionWithBox(const CollisionEvent &event) override;

    void handleCollisionWithCircle(const CollisionEvent &event) override;
};



