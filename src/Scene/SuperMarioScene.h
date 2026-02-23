//
// Created by MINEC on 2026/1/29.
//

#pragma once
#include "BoxGameObject.h"
#include "Button.h"
#include "Ground.h"
#include "Scene.h"
#include "CollisionSystem.h"
#include "Mario.h"
#include "SimpleNetwork.h"


class SuperMarioScene : public Scene {
public:
    explicit SuperMarioScene(sf::RenderWindow* _window) : Scene(_window, "SuperMarioScene") {}
    ~SuperMarioScene() override = default;

    void init() override {
        Scene::init();
        if (is_init) return;
        is_init = true;
        collisionSystem = std::make_unique<CollisionSystem>();
        AssetManager::getInstance().loadTexture("./Asset/SuperMario/resources/graphics");
        FrameManager::getInstance().loadFrame();

        bg.setTexture(AssetManager::getInstance().getTexture("level_1"));
        const float bg_scale = static_cast<float>(window->getSize().y) / bg.getLocalBounds().height;
        bg.setScale(bg_scale, bg_scale);
        initStaticObjects();
    }

    void initStaticObjects() {
        // 左墙
        std::shared_ptr<Ground> wall1 = std::make_shared<Ground>(0, 0, 10, 960, "wall1");
        this->addObject(wall1);
        // 地板
        std::shared_ptr<Ground> ground = std::make_shared<Ground>(-10000, 857, 120000, 800);
        this->addObject(ground);

        std::shared_ptr<Ground> pipe1 = std::make_shared<Ground>(1927, 720, 124, 137);
        this->addObject(pipe1);

        std::shared_ptr<Ground> box2 = std::make_shared<Ground>(333, 652, 100, 100);
        this->addObject(box2);
    }

    void initDynamicObjects() {
        if (is_initDynamicObjects) return;
        is_initDynamicObjects = true;
        std::shared_ptr<Mario> mario = std::make_shared<Mario>(100.f, 100.f);
        this->addObjectWithNetwork(mario);

        std::shared_ptr<Player> player2 = std::make_shared<Player>(60, 300, 40);
        player2->removeComponent<GravityComponent>();
        // player2->addComponent<CameraComponent>();
        this->addObjectWithNetwork(player2);


        std::shared_ptr<BoxGameObject> box = std::make_shared<BoxGameObject>(800, 800, 300, 80);
        const auto move = box->addComponent<MoveComponent>();
        move->setSpeedX(-200);
        // box->addComponent<GravityComponent>();
        this->addObjectWithNetwork(box);
    }

    void render(sf::RenderWindow* _window) override {
        _window->draw(bg);
        Scene::render(_window);
    }

    void update(sf::Time deltaTime) override {
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

    void handleEvent(sf::Event& event) override {
        simple_network.handleEvent(event);
        Scene::handleEvent(event);
        if (event.type == sf::Event::MouseButtonPressed) {
            const sf::Vector2i pos = SceneContext::getInstance().getMousePosition();
            std::cout << "SuperMarioScene:" << pos.x << " " << pos.y << std::endl;
        } else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                SceneContext::getInstance().getSceneManager()->loadScene("MenuScene");
            }
        }
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

private:
    std::unique_ptr<CollisionSystem> collisionSystem;
    SimpleNetwork simple_network;
    sf::Sprite bg;
    bool is_initDynamicObjects = false;
};
