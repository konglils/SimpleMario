//
// Created by MINEC on 2025/12/11.
//

#pragma once
#include "CollisionHandle.h"

class BoxCollisionHandle : public CollisionHandle {
public:
    BoxCollisionHandle();
    // 目前两种处理都是一样的
    void handleCollisionWithBox(const CollisionEvent &event) override;

    void handleCollisionWithCircle(const CollisionEvent &event) override;
};
