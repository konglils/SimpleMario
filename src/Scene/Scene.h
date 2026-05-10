//
// Created by MINEC on 2026/1/2.
//
#pragma once
#include <memory>
#include <utility>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "GameObject.h"
#include "SceneContext.h"
#include "Camera.h"
#include "NetworkManager.h"

class CollisionSystem;
class SceneManager;
class Scene {
public:
    explicit Scene(sf::RenderWindow* _window) : window(_window), scene_name("Scene") {}
    explicit Scene(sf::RenderWindow* _window, std::string _name) : window(_window), scene_name(std::move(_name)) {}
    virtual ~Scene() = default;

    // 场景初始化方法
    virtual void init() {
        this->setCamera(window);
        this->setSceneContext();
    }

    virtual void exit() {

    }

    virtual std::shared_ptr<GameObject> spawnEntity() {
        return nullptr;
    }

    virtual std::shared_ptr<GameObject> spawnEntity(sf::Packet& packet) {
        return nullptr;
    }

    // 场景更新方法
    virtual void update(sf::Time deltaTime) {
        // 删除已销毁的 GameObject
        game_objects.erase(
            std::remove_if(game_objects.begin(), game_objects.end(), [](const auto& obj) {
                return obj->isDestroy();
            }),
            game_objects.end()
        );
        for (auto it = game_objects_map.begin(); it != game_objects_map.end(); ) {
            if (it->second->isDestroy()) {
                it = game_objects_map.erase(it);
            } else {
                ++it;
            }
        }
        // 必须用这种 for 循环，因为 game_objects 可能会改变，扩容导致迭代器失效
        for (int i = 0; i < game_objects.size(); ++i) {
            if (const auto& obj = game_objects[i]; obj->isActive()) {
                if (obj->hasStarted()) obj->update(deltaTime);
                else obj->start();
            }
        }
    }

    // 场景渲染方法
    virtual void render(sf::RenderWindow* _window) {
        for (const auto& obj : game_objects) {
            if (obj->isActive()) {
                obj->render(_window);
            }
        }
    }

    // 场景事件处理方法
    virtual void handleEvent(sf::Event& event) {
        if (camera) camera->handleEvent(event);
        // 必须用这种 for 循环，因为 game_objects 可能会改变，扩容导致迭代器失效
        for (int i = 0; i < game_objects.size(); ++i) {
            const auto& obj = game_objects[i];
            obj->handleEvent(event);
        }
        if (camera && event.type == sf::Event::Resized) {
            camera->resize();
        }
    }

    // 游戏对象管理
    std::vector<std::shared_ptr<GameObject>>& getGameObjects() {
        return game_objects;
    }

    virtual void addObject(const std::shared_ptr<GameObject>& obj) {
        game_objects.push_back(obj);
    }

    virtual void addObjectWithMap(const std::shared_ptr<GameObject>& obj) {
        game_objects.push_back(obj);
        game_objects_map[obj->getId()] = obj;
    }

    virtual void addObjectWithNetwork(const std::shared_ptr<GameObject>& obj) {

    }

    std::shared_ptr<GameObject> findGameObjectById(const unsigned int id) {
        if (game_objects_map.find(id) == game_objects_map.end()) {
            std::cerr << "Scene::findGameObjectById : GameObject with ID " << id << " are not found" << std::endl;
            return nullptr;
        }
        return game_objects_map[id];
    }

    void removeObjectById(const unsigned int id) {
        if (game_objects_map.find(id) == game_objects_map.end()) {
            std::cerr << "Scene::removeObjectById : GameObject with id " << id << " are not found" << std::endl;
            return;
        }
        game_objects_map.erase(id);
        for (auto it = game_objects.begin(); it != game_objects.end(); ++it) {
            if ((*it)->getId() == id) {
                game_objects.erase(it);
                std::cout << "Scene::removeObjectById : Removing GameObject with id " << id << std::endl;
                break;
            }
        }
    }

    // 相机管理
    void setCamera(sf::RenderWindow* _window) {
        camera = std::make_unique<Camera>(_window);
    }

    [[nodiscard]] Camera* getCamera() const {
        return camera.get();
    }

    // 场景大小和上下文管理
    void setSceneContext() const {
        if (window) SceneContext::getInstance().setWindow(window);
        if (camera) SceneContext::getInstance().setCamera(camera.get());
        SceneContext::getInstance().setGameObjects(&game_objects);
        SceneContext::getInstance().setSceneManager(scene_manager);
    }

    [[nodiscard]] sf::Vector2u getWindowSize() const {
        return window->getSize();
    }

    [[nodiscard]] const std::string& getSceneName() const {
        return scene_name;
    }

    void setSceneManager(SceneManager* _scene_manager) {
        scene_manager = _scene_manager;
        SceneContext::getInstance().setSceneManager(_scene_manager);
    }

    [[nodiscard]] virtual CollisionSystem* getCollisionSystem() const {
        return nullptr;
    }

    virtual NetworkManager::NetworkType getNetworkType() const {
        return NetworkManager::NetworkType::None;
    }

protected:
    std::vector<std::shared_ptr<GameObject>> game_objects;
    std::unordered_map<unsigned int, std::shared_ptr<GameObject>> game_objects_map;
    sf::RenderWindow* window{};
    std::unique_ptr<Camera> camera;
    std::string scene_name;
    SceneManager* scene_manager{};
    bool is_init = false;
};
