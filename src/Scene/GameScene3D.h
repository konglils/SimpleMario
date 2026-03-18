//
// Created by MINEC on 2026/1/3.
//

#pragma once

#include "Scene.h"
#include "Cube3D.h"
#include "NewModel3D.h"
#include "Penguin3D.h"
#include "Human3D.h"

class GameScene3D : public Scene {
public:
    explicit GameScene3D(sf::RenderWindow* _window) : Scene(_window, "GameScene3D") {}
    ~GameScene3D() override = default;

    void init() override {
        window->setSize(sf::Vector2u(1200, 1200));
        Scene::init();
        if (is_init) return;
        is_init = true;
        // this->addObject(std::make_shared<Penguin3D>());
        // this->addObject(std::make_shared<Cube3D>());
        // this->addObject(std::make_shared<NewModel3D>());
        this->addObject(std::make_shared<Human3D>());
    }

    void handleEvent(sf::Event& event) override {
        Scene::handleEvent(event);
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            SceneContext::getInstance().getSceneManager()->loadScene("MenuScene");
        }
    }

    void exit() override {
        window->setSize(sf::Vector2u(1200, 960));
    }
};
