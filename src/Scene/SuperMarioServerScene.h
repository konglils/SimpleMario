//
// Created by MINEC on 2026/5/11.
//

#pragma once
#include "Scene.h"
#include <memory>
#include "Mario.h"
#include "CollisionSystem.h"
#include "Ground.h"
#include "Box.h"
#include "Brick.h"

class SuperMarioServerScene : public Scene {
public:
    explicit SuperMarioServerScene() : Scene("SuperMarioScene") { }

    ~SuperMarioServerScene() override = default;

    void init() override {
        Scene::init();
        if (is_init) return;
        is_init = true;
        collisionSystem = std::make_unique<CollisionSystem>();

        initStaticObjects();

        startServer();
    }

    std::shared_ptr<GameObject> spawnEntity() override {
        auto obj = std::make_shared<Mario>(100.f, 100.f, false);
        this->addObjectWithNetwork(obj);
        return obj;
    }

    std::shared_ptr<GameObject> spawnEntity(sf::Packet& packet) override {
        unsigned int id;
        ObjectType obj_type;
        packet >> id >> obj_type;
        if (obj_type == ObjectType::MarioPlayer || obj_type == ObjectType::Mario) {
            float x, y, s_x, s_y;
            bool is_jump;
            packet >> x >> y >> s_x >> s_y >> is_jump;
            const auto player = std::make_shared<Mario>(x, y, obj_type == ObjectType::MarioPlayer);
            player->setId(id);
            LOG_DEBUG_FMT("Create mario, id:{}, x:{}, y:{}, s_x:{}, s_y:{}, is_jump:{}", id, x, y, s_x, s_y, is_jump);
            const auto& move_component = player->getComponent<MoveComponent>();
            move_component->setSpeed(s_x, s_y);
            this->addObjectWithNetwork(player);
            return player;
        }
        if (obj_type == ObjectType::FireBall) {
            unsigned int owner_id;
            float x, y, s_x, s_y;
            packet >> owner_id >> x >> y >> s_x >> s_y;
            const auto fire_ball = std::make_shared<FireBall>(owner_id, x, y);
            fire_ball->setId(id);
            fire_ball->getComponent<MoveComponent>()->setSpeed(sf::Vector2f(s_x, s_y));
            this->addObjectWithNetwork(fire_ball);
            return fire_ball;
        }
        LOG_ERROR("Invalid object type");
        return nullptr;
    }

    void initStaticObjects() {
        // 左墙
        std::shared_ptr<Ground> wall1 = std::make_shared<Ground>(0, 0, 10, CONFIG.window.height, "wall1");
        this->addObject(wall1);

        std::vector<std::pair<int, int>> bricks = {
            {1154, 609}, {1429, 609}, {1557, 609}, {1621, 609},
            {13186, 571}
        };

        this->addObject(std::make_shared<Box>(1493, 609));

        for (const auto& [x, y] : bricks) {
            this->addObject(std::make_shared<Brick>(x, y));
        }

        std::vector<std::array<int, 4>> collisions = {
            {1927, 722, 2053, 852}, {2612, 655, 2738, 853}, {3163, 586, 3287, 851}, {3914, 585, 4042, 848},
            {9188, 789, 9256, 854}, {9256, 721, 9321, 853}, {9325, 651, 9389, 851}, {9395, 583, 9459, 851},
            {9599, 584, 9664, 851}, {9668, 651, 9734, 852}, {9738, 721, 9803, 853}, {9805, 790, 9873, 852},
            {10149, 789, 10212, 852}, {10217, 720, 10282, 853}, {10284, 651, 10352, 852}, {10355, 584, 10490, 852},
            {10629, 585, 10694, 851}, {10698, 652, 10761, 852}, {10764, 720, 10832, 850}, {10834, 789, 10901, 853},
            {11183, 723, 11310, 852}, {12280, 720, 12406, 853},
            {12412, 789, 12478, 852}, {12481, 720, 12544, 851}, {12547, 651, 12615, 852}, {12617, 583, 12682, 850},
            {12687, 513, 12752, 852}, {12755, 448, 12819, 850}, {12824, 378, 12889, 851}, {12892, 310, 13025, 853},
            {0, 857, 4728, 2000}, {4870, 858, 5893, 2000}, {6104, 859, 10489, 2000}, {10628, 857, 14535, 2000}
        };

        for (const auto [x1, y1, x2, y2] : collisions) {
            this->addObject(std::make_shared<Ground>(x1, y1, x2 - x1, y2 - y1));
        }
    }

    void initDynamicObjects() {
        if (is_initDynamicObjects) return;
        is_initDynamicObjects = true;
        // std::shared_ptr<Mario> mario = std::make_shared<Mario>(100.f, 100.f);
        // this->addObjectWithNetwork(mario);
    }

    void update(sf::Time deltaTime) override {
        LOG_TRACE_FMT("deltaTime: {} ms", deltaTime.asMilliseconds());
        Scene::update(deltaTime);
        if (this->collisionSystem) {
            this->collisionSystem->checkCollisions();
        }
        simple_network.update(deltaTime);
    }

    void addObject(const std::shared_ptr<GameObject>& obj) override {
        Scene::addObject(obj);
        if (this->collisionSystem && obj->getComponent<Collision>()) {
            this->collisionSystem->addObject(obj);
        }
    }

    void addObjectWithMap(const std::shared_ptr<GameObject>& obj) override {
        Scene::addObjectWithMap(obj);
        if (this->collisionSystem && obj->getComponent<Collision>()) {
            this->collisionSystem->addObject(obj);
        }
    }

    void addObjectWithNetwork(const std::shared_ptr<GameObject>& obj) override {
        addObjectWithMap(obj);
        simple_network.addGameObject(obj);
    }

    CollisionSystem* getCollisionSystem() const override {
        return collisionSystem.get();
    }

    void startServer() {
        if (simple_network.startServer()) {
            initDynamicObjects();
        }
    }

    void connectToServer(const std::string& address) {
        simple_network.connectToServer(address);
    }

    NetworkManager::NetworkType getNetworkType() const override {
        return simple_network.getNetworkType();
    }

private:
    std::unique_ptr<CollisionSystem> collisionSystem;
    NetworkManager simple_network;
#ifndef SERVER_BUILD
    sf::Sprite bg;
#endif
    bool is_initDynamicObjects = false;
};

