//
// Created by MINEC on 2025/12/11.
//

#pragma once
#include <functional>
#include <unordered_map>
#include "Component.h"
#include "Events.h"

class CollisionHandle : public Component {
public:
    void handleCollision(const CollisionEvent& event);

    virtual void handleCollisionWithBox(const CollisionEvent& event) = 0;
    virtual void handleCollisionWithCircle(const CollisionEvent& event) = 0;

    static void handle(const CollisionEvent& event);

protected:
    std::unordered_map<size_t, std::function<void(CollisionEvent)>> collisionHandlers;
};
