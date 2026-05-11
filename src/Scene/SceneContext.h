//
// Created by MINEC on 2025/12/10.
//

#pragma once
#include "Camera.h"
#include "GameObject.h"
#include <SFML/Graphics.hpp>

#include "ConfigManager.h"

class SceneManager;

class SceneContext {
public:
    static SceneContext& getInstance() {
        static SceneContext instance;
        return instance;
    }
#ifndef SERVER_BUILD
    void setCamera(Camera* _camera) {
        this->camera = _camera;
        this->camera->init();
    }

    void setWindow(sf::RenderWindow* _window) {
        this->window = _window;
    }
#endif
    void setGameObjects(const std::vector<std::shared_ptr<GameObject>>* _game_objects) {
        game_objects = _game_objects;
    }

    void setSceneManager(SceneManager* _scene_manager) {
        scene_manager = _scene_manager;
    }
#ifndef SERVER_BUILD
    [[nodiscard]] Camera* getCamera() const {
        return camera;
    }

    [[nodiscard]] sf::RenderWindow* getWindow() const {
        return window;
    }
#endif

    [[nodiscard]] unsigned int getWindowWidth() const {
#ifndef SERVER_BUILD
        if (window) return window->getSize().x;
#endif
        return CONFIG.window.width;
    }

    [[nodiscard]] unsigned int getWindowHeight() const {
#ifndef SERVER_BUILD
        if (window) return window->getSize().y;
#endif
        return CONFIG.window.height;
    }

    [[nodiscard]] const std::vector<std::shared_ptr<GameObject>>* getGameObjects() const {
        return game_objects;
    }

    [[nodiscard]] SceneManager* getSceneManager() const {
        return scene_manager;
    }
#ifndef SERVER_BUILD
    sf::Vector2i getMousePosition() const {
        const sf::Vector2f camera_center = camera->getCenter();
        const sf::Vector2u window_size = window->getSize();
        sf::Vector2i mouse_position = sf::Mouse::getPosition(*window);
        mouse_position.x += camera_center.x - window_size.x * 0.5f;
        mouse_position.y += camera_center.y - window_size.y * 0.5f;
        return mouse_position;
    }
#endif

private:
#ifndef SERVER_BUILD
    sf::RenderWindow* window{};
    Camera* camera{};
#endif
    const std::vector<std::shared_ptr<GameObject>>* game_objects{};
    SceneManager* scene_manager{};
};

