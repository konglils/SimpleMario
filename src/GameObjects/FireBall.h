//
// Created by MINEC on 2026/5/7.
//

#pragma once
#include "Animation.h"
#include "GameObject.h"
#include "Events.h"



class FireBall : public GameObject {
public:
    FireBall(GameObject* owner, float x, float y, float speed_x = 0.f);

    ~FireBall() override;

    void start() override;

    void render(sf::RenderWindow* window) override;

    void update(sf::Time deltaTime) override;

    void setExploded();

    void handleCollision(const CollisionEvent& event);

private:
    bool is_exploded = false;
    Animation animation;
    Animation explosionAnimation;
    GameObject* owner{nullptr};
};
