//
// Created by MINEC on 2026/5/8.
//

#include "FireBall.h"

#include "FrameManager.h"
#include "GravityComponent.h"
#include "Collision.h"
#include "EventBus.h"
#include "BoxCollision.h"

FireBall::FireBall(GameObject* owner, const float x, const float y, const float speed_x) {
    this->owner = owner;
    this->position = sf::Vector2f(x, y);

    animation.setFrames(FrameManager::getInstance().getFrame("fireball_frame"));
    explosionAnimation.setFrames(FrameManager::getInstance().getFrame("explosion_frame"));
    this->setSize(animation.getFrameWidth(), animation.getFrameHeight());

    this->addComponent<Collision, BoxCollision, true>();
    this->addComponent<GravityComponent>();

    this->addComponent<MoveComponent>()->setSpeed(sf::Vector2f(speed_x, -400.f));

    this->tag = "fireball:" + std::to_string(this->id);
    className = "FireBall";
}

FireBall::~FireBall() {
    EventBus::getInstance().removeSubscribe("onCollision" + this->tag);
}

void FireBall::start() {
    GameObject::start();
    EventBus::getInstance().subscribe<CollisionEvent>(
        "onCollision" + this->tag,
        [this](const CollisionEvent& collisionEvent) {
            handleCollision(collisionEvent);
        }
    );
}

void FireBall::render(sf::RenderWindow* window) {
    GameObject::render(window);
    if (is_exploded) explosionAnimation.render(window, this->position);
    else animation.render(window, this->position);
}

void FireBall::update(sf::Time deltaTime) {
    GameObject::update(deltaTime);
    if (is_exploded) {
        explosionAnimation.update(deltaTime);
        if (explosionAnimation.isOver()) {
            destroy();
        }
    } else {
        animation.update(deltaTime);
    }
}

void FireBall::setExploded() {
    is_exploded = true;
    this->getComponent<GravityComponent>()->setActive(false);
    this->getComponent<MoveComponent>()->setActive(false);
    this->getComponent<MoveComponent>()->addPosition(sf::Vector2f(-16.f, -16.f), false);
}

void FireBall::handleCollision(const CollisionEvent& event) {
    auto& this_ = event.a;
        auto& other = event.b;

        if (owner == other.get()) return;

        // std::cout << this_->getTag() << ' ' << other->getTag() << std::endl;

        if (!this_->getMoveAble()) return;
        std::shared_ptr<MoveComponent> moveComponent = this_->getComponent<MoveComponent>();
        if (!moveComponent) return;

        // 计算 x 方向和 y 方向的重合度
        const float dx = std::min(event.a_position.x + this_->getSize().x,
            event.b_position.x + other->getSize().x) - std::max(event.a_position.x, event.b_position.x);
        const float dy = std::min(event.a_position.y + this_->getSize().y,
            event.b_position.y + other->getSize().y) - std::max(event.a_position.y, event.b_position.y);

        // 水平碰撞
        if (dx <= dy) {
            float right_x = std::abs(event.a_position.x + this_->getSize().x - (event.b_position.x + other->getSize().x * 0.5f));
            float left_x = std::abs(event.a_position.x - (event.b_position.x + other->getSize().x * 0.5f));
            if (right_x < left_x) {
                moveComponent->moveCollisionXTo(event.b_position.x - this_->getSize().x);
            } else {
                moveComponent->moveCollisionXTo(event.b_position.x + other->getSize().x);
            }

            // 设置为爆炸
            setExploded();
        } else {
            // 处理垂直方向碰撞：当火球向上运动且重叠区域差异较小时忽略碰撞
            if (this_->getSpeed().y < 0 && dx - dy < 10.f) return;

            const float relativeSpeedY = event.b_speed.y - event.a_speed.y;
            moveComponent->setSpeedY(relativeSpeedY * 0.5f);
            if (relativeSpeedY < 0) {
                moveComponent->setSpeedY(std::min(relativeSpeedY * 0.5f, -500.f));
            }

            // 计算火球顶部和底部与碰撞物体的距离，判断碰撞面并调整位置
            float top_y = std::abs(event.a_position.y - (event.b_position.y + other->getSize().y * 0.5f));
            float bottom_y = std::abs(event.a_position.y + this_->getSize().y - (event.b_position.y + other->getSize().y * 0.5f));
            if (top_y > bottom_y) {
                moveComponent->moveCollisionYTo(event.b_position.y - this_->getSize().y);
            } else {
                moveComponent->moveCollisionYTo(event.b_position.y + other->getSize().y);
            }
        }
}
