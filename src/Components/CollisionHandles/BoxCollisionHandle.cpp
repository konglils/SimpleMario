//
// Created by MINEC on 2026/5/8.
//

#include "BoxCollisionHandle.h"

#include "BoxCollision.h"

BoxCollisionHandle::BoxCollisionHandle() {
    collisionHandlers[typeid(BoxCollision).hash_code()] = [this](const CollisionEvent& event) {
        this->handleCollisionWithBox(event);
    };
    collisionHandlers[typeid(CircleCollision).hash_code()] = [this](const CollisionEvent& event) {
        this->handleCollisionWithCircle(event);
    };
}

void BoxCollisionHandle::handleCollisionWithBox(const CollisionEvent& event) {
    handle(event);
}

void BoxCollisionHandle::handleCollisionWithCircle(const CollisionEvent& event) {
    handle(event);
}
