//
// Created by MINEC on 2025/12/10.
//

#pragma once

#include "Component.h"
#include "GameObject.h"
#include "Collision.h"
#include <cmath>

class MoveComponent : public Component {
public:
    MoveComponent() = default;
    void update(const sf::Time& deltaTime) override {
        owner->position += owner->speed * deltaTime.asSeconds();
        setPosition(owner->position.x, owner->position.y);
    }

    void render(sf::RenderWindow* window) override {
        if (owner->speed.x == 0.f && owner->speed.y == 0.f) return;
        const auto center = owner->getCenter();
        drawArrow(window, center.x, center.y, center.x + owner->speed.x / 10.f, center.y + owner->speed.y / 10.f);
    }

    void setPosition(const sf::Vector2f& pos, const bool move_collision = true) const {
        owner->setPosition(pos.x, pos.y);
        if (move_collision) setCollisionPosition(pos);
    }

    void moveCollisionTo(const sf::Vector2f& pos) const {
        if (const auto& collision = owner->getComponent<Collision>()) {
            const auto delta_pos = pos - collision->getCollisionPosition();
            collision->setCollisionPosition(pos);
            this->addPosition(delta_pos, false);
        }
    }

    void moveCollisionXTo(const float posX) const {
        const auto& collision = owner->getComponent<Collision>();
        moveCollisionTo(sf::Vector2f(posX, collision->getCollisionPosition().y));
    }

    void moveCollisionYTo(const float posY) const {
        const auto& collision = owner->getComponent<Collision>();
        moveCollisionTo(sf::Vector2f(collision->getCollisionPosition().x, posY));
    }

    void setPosition(const float posX, const float posY, const bool move_collision = true) const {
        setPosition(sf::Vector2f(posX, posY), move_collision);
    }

    void setPositionX(const float posX, const bool move_collision = true) const {
        setPosition(sf::Vector2f(posX, owner->position.y), move_collision);
    }

    void setPositionY(const float posY, const bool move_collision = true) const {
        setPosition(sf::Vector2f(owner->position.x, posY), move_collision);
    }

    void addPosition(const sf::Vector2f& pos, const bool move_collision = true) const {
        setPosition(owner->position + pos, move_collision);
    }

    void setSpeed(const sf::Vector2f& speed) const {
        owner->speed = speed;
    }

    void setSpeedX(const float speedX) const {
        owner->speed.x = speedX;
    }

    void setSpeedY(const float speedY) const {
        owner->speed.y = speedY;
    }

    void setSpeed(const float speedX, const float speedY) const {
        owner->speed = sf::Vector2f(speedX, speedY);
    }

    void addSpeed(const sf::Vector2f& speed) const {
        owner->speed += speed;
    }

    static void drawArrow(sf::RenderWindow* window, const float x1, const float y1, const float x2, const float y2,
                   const float arrowSize = 10.0f, const sf::Color color = sf::Color::Red) {
        // 绘制箭杆
        const sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(x1, y1), color),
            sf::Vertex(sf::Vector2f(x2, y2), color)
        };
        window->draw(line, 2, sf::Lines);

        // 计算箭头方向角度
        const float angle = std::atan2(y2 - y1, x2 - x1);

        // 计算箭头头部的两个翼点
        constexpr auto pi_6 = static_cast<float>(M_PI / 6);
        const float x3 = x2 - arrowSize * cosf(angle + pi_6);
        const float y3 = y2 - arrowSize * sinf(angle + pi_6);
        const float x4 = x2 - arrowSize * cosf(angle - pi_6);
        const float y4 = y2 - arrowSize * sinf(angle - pi_6);

        // 绘制箭头头部（三角形）
        const sf::Vertex arrowHead[] = {
            sf::Vertex(sf::Vector2f(x2, y2), color),
            sf::Vertex(sf::Vector2f(x3, y3), color),
            sf::Vertex(sf::Vector2f(x4, y4), color)
        };
        window->draw(arrowHead, 3, sf::Triangles);
    }

private:
    void setCollisionPosition(const sf::Vector2f& pos) const {
        if (const auto& collision = owner->getComponent<Collision>()) {
            collision->setPosition(pos);
        }
    }
};


