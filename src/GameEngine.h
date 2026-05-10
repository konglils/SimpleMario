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
#include "Logger.h"


class GameEngine {
public:
    GameEngine() = default;
    ~GameEngine() {
        delete window;
    }

    void init() {
        Logger::getInstance().setLogFile("log.txt");
        Logger::getInstance().setLogLevel(LogLevel::Debug); // 只显示 Debug 及以上级别

        LOG_INFO("GAME START!");

        if (!CONFIG.load()) {
            LOG_WARN("Config file load fail, using default config");
        }
        // 加载 SuperMarioScene 的资源
        LOG_INFO("Loading SuperMarioScene resources...");
        AssetManager::getInstance().loadTexture(CONFIG.getTexturePath("superMario").c_str());
        AssetManager::getInstance().loadSoundBuffer(CONFIG.getSoundPath("superMario").c_str());
        FrameManager::getInstance().loadFrame();
        LOG_INFO("SuperMarioScene resources loaded.");

        if (!window) window = new sf::RenderWindow(
            sf::VideoMode(CONFIG.window.width, CONFIG.window.height), CONFIG.window.title);
        scene_manager = std::make_shared<SceneManager>();
        scene_manager->addScene<GameScene>(window);
        scene_manager->addScene<GameScene3D>(window);
        scene_manager->addScene<SuperMarioScene>(window);
        scene_manager->addScene<MenuScene>(window);
        scene_manager->loadScene("MenuScene");
    }

    void start() const {
        window->setFramerateLimit(CONFIG.window.fps);
        sf::Clock clock;
        while (window->isOpen()) {
            const sf::Time deltaTime = clock.restart();
            sf::Event event{};
            while (window->pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window->close();
                    break;
                }
                scene_manager->handleEvent(event);
            }
            if (!window->isOpen()) break;
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

