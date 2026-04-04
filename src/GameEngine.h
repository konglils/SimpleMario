//
// Created by MINEC on 2025/12/9.
//
#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

#include "ConfigManager.h"
#include "GameScene.h"
#include "GameScene3D.h"
#include "MenuScene.h"
#include "SceneManager.h"
#include "SuperMarioScene.h"


class GameEngine {
public:
    GameEngine() = default;
    ~GameEngine() {
        delete window;
    }

    void init() {
        auto& config = ConfigManager::getInstance();
        if (!config.load()) {
            std::cerr << "Using default config" << std::endl;
        }
        // 加载 SuperMarioScene 的资源
        std::cout << "Loading SuperMarioScene resources..." << std::endl;
        AssetManager::getInstance().loadTexture(config.getTexturePath("superMario").c_str());
        AssetManager::getInstance().loadSoundBuffer(config.getSoundPath("superMario").c_str());
        FrameManager::getInstance().loadFrame();
        std::cout << "SuperMarioScene resources loaded." << std::endl;

        if (!window) window = new sf::RenderWindow(
            sf::VideoMode(config.window.width, config.window.height), config.window.title);
        scene_manager = std::make_shared<SceneManager>();
        scene_manager->addScene<GameScene>(window);
        scene_manager->addScene<GameScene3D>(window);
        scene_manager->addScene<SuperMarioScene>(window);
        scene_manager->addScene<MenuScene>(window);
        scene_manager->loadScene("MenuScene");
    }

    void start() const {
        window->setFramerateLimit(ConfigManager::getInstance().window.fps);
        sf::Clock clock;
        while (window->isOpen()) {
            const sf::Time deltaTime = clock.restart();
            sf::Event event{};
            while (window->pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window->close();
                }
                scene_manager->handleEvent(event);
            }

            scene_manager->update(deltaTime);
            window->clear();
            scene_manager->render(window);
            window->display();
        }
    }

private:
    std::shared_ptr<SceneManager> scene_manager;
    sf::RenderWindow* window{};
};

