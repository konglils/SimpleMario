//
// Created by MINEC on 2026/2/20.
//

#pragma once
#include <SFML/Network.hpp>
#include <iostream>
#include "Mario.h"
#include "nlohmann/json.hpp"

class SimpleNetwork {
public:
    enum class NetworkType {
        None,
        Server,
        Client
    };
    SimpleNetwork() = default;
    ~SimpleNetwork() = default;

    bool startServer() {
        if (network_type == NetworkType::Server) return true;
        if (network_type == NetworkType::Client) {
            std::cout << "Cannot start server while already running as a client!" << std::endl;
            return false;
        }
        std::cout << "Starting server on port " << port << "..." << std::endl;

        if (listener.listen(port) != sf::Socket::Done) {
            std::cout << "Failed to start server" << std::endl;
            return false;
        }

        listener.setBlocking(false);
        network_type = NetworkType::Server;
        std::cout << "Server started successfully!" << std::endl;
        return true;
    }

    bool connectToServer(const std::string& address) {
        if (network_type == NetworkType::Client) return true;
        if (network_type == NetworkType::Server) {
            std::cout << "Cannot connect to server while already running as a server!" << std::endl;
            return false;
        }
        std::cout << "Connecting to " << address << ":" << port << "..." << std::endl;

        if (clientSocket.connect(address, port, sf::seconds(5)) != sf::Socket::Done) {
            std::cout << "Failed to connect to server!" << std::endl;
            return false;
        }

        clientSocket.setBlocking(false);
        network_type = NetworkType::Client;
        std::cout << "Connected to server successfully!" << std::endl;
        return true;
    }

    void update(const sf::Time& deltaTime) {
        if (network_type == NetworkType::None) return;
        if (network_type == NetworkType::Server) {
            serverUpdate(deltaTime);
        } else {
            clientUpdate(deltaTime);
        }
    }

    void handleEvent(const sf::Event& event) {
        if (network_type != NetworkType::None && event.type == sf::Event::Closed) {
            if (network_type == NetworkType::Server) {
                for (const auto& client : clients) {
                    client->disconnect();
                }
                clients.clear();
            } else {
                clientSocket.disconnect();
            }
        }
        if (network_type != NetworkType::Client) return;
        sf::Packet packet;
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::W) {
                packet << 0;
                clientSocket.send(packet);
            } else if (event.key.code == sf::Keyboard::A) {
                packet << 1;
                clientSocket.send(packet);
            } else if (event.key.code == sf::Keyboard::D) {
                packet << 2;
                clientSocket.send(packet);
            }
        } else if (event.type == sf::Event::KeyReleased) {
            if (event.key.code == sf::Keyboard::A) {
                packet << 3;
                clientSocket.send(packet);
            } else if (event.key.code == sf::Keyboard::D) {
                packet << 4;
                clientSocket.send(packet);
            }
        }
    }

    void serverUpdate(const sf::Time& deltaTime) {
        // 接收新用户连接
        if (const auto newClient = std::make_shared<sf::TcpSocket>(); listener.accept(*newClient) == sf::Socket::Done) {
            newClient->setBlocking(false);

            // 给客户端发送当前场景信息
            for (const auto& obj : game_objects) {
                sf::Packet packet;
                packet << 0;
                if (obj->getClassName() == "Mario") {
                    const bool is_jump = obj->getComponent<StateMachine>()->getCurrentStateName() == "MarioJumpState";
                    packet << 1 << obj->getPosition().x << obj->getPosition().y << obj->getSpeed().x << obj->getSpeed().y << is_jump;
                } else if (obj->getClassName() == "Player") {
                    packet << 2 << obj->getPosition().x << obj->getPosition().y << obj->getSpeed().x << obj->getSpeed().y << obj->getSize().x / 2;
                } else if (obj->getClassName() == "BoxGameObject") {
                    packet << 3 << obj->getPosition().x << obj->getPosition().y << obj->getSpeed().x << obj->getSpeed().y << obj->getSize().x << obj->getSize().y;
                }
                packet << obj->getId();
                newClient->send(packet);
            }
            // 创建新加入的玩家
            const auto current_scene = SceneContext::getInstance().getSceneManager()->getCurrentScene();
            const auto newPlayer = std::make_shared<Mario>(100.f, 100.f, false);
            current_scene->addObjectWithNetwork(newPlayer);
            players[newClient.get()] = newPlayer;

            // 发送新玩家信息给所有玩家
            sf::Packet packet;
            packet << 0;
            const bool is_jump = newPlayer->getComponent<StateMachine>()->getCurrentStateName() == "MarioJumpState";
            packet << 1 << newPlayer->getPosition().x << newPlayer->getPosition().y << newPlayer->getSpeed().x << newPlayer->getSpeed().y << is_jump;
            packet << newPlayer->getId();

            for (const auto& client : clients) {
                client->send(packet);
            }

            // 发送新玩家信息给新玩家自己
            packet.clear();
            packet << 0;
            packet << 0 << newPlayer->getPosition().x << newPlayer->getPosition().y << newPlayer->getSpeed().x << newPlayer->getSpeed().y << is_jump;
            packet << newPlayer->getId();
            newClient->send(packet);

            clients.emplace_back(newClient);

            std::cout << "New client connected!" << std::endl;
        }

        // 处理客户端数据
        std::unordered_map<unsigned int, bool> removeIdsMap;
        for (auto it = clients.begin(); it != clients.end();) {
            const auto& client = *it;
            sf::Packet packet;
            sf::Socket::Status status = client->receive(packet);
            if (status == sf::Socket::Error || status == sf::Socket::Disconnected) {
                const unsigned int id = players[client.get()]->getId();
                removeIdsMap[id] = true;
                players.erase(client.get());
                // 删除玩家
                for (auto obj_it = game_objects.begin(); obj_it != game_objects.end(); ++obj_it) {
                    if ((*obj_it)->getId() == id) {
                        game_objects.erase(obj_it);
                        break;
                    }
                }
                SceneContext::getInstance().getSceneManager()->getCurrentScene()->removeObjectById(id);
                it = clients.erase(it);
                continue;
            }
            while (status == sf::Socket::Done) {
                const auto& marioController = players[client.get()]->getComponent<MarioController>();
                int type;
                packet >> type;
                if (type == 0) {
                    marioController->jump();
                    players[client.get()]->getComponent<StateMachine>()->setState("MarioJumpState");
                } else if (type == 1) {
                    marioController->runLeft();
                } else if (type == 2) {
                    marioController->runRight();
                } else if (type == 3 || type == 4) {
                    marioController->stopRun();
                }

                status = client->receive(packet);
            }
            ++it;
        }
        // 通知所有在线玩家删除不在线的玩家
        for (const auto& client : clients) {
            for (const auto& [id, remove] : removeIdsMap) {
                sf::Packet packet;
                packet << 2 << id;
                client->send(packet);
            }
        }

        // 删除不在线的玩家在碰撞系统中的引用
        const auto collision_objects = SceneContext::getInstance().getSceneManager()->getCurrentScene()->getCollisionSystem()->getObjects();
        for (auto it = collision_objects->begin(); it != collision_objects->end();) {
            if (removeIdsMap.find((*it)->getId()) != removeIdsMap.end()) {
                it = collision_objects->erase(it);
            } else {
                ++it;
            }
        }

        // 向客户端同步数据
        for (const auto& client : clients) {
            for (const auto& obj : game_objects) {
                sf::Packet packet;
                packet << 1;
                if (obj->getClassName() == "Mario") {
                    const bool is_jump = obj->getComponent<StateMachine>()->getCurrentStateName() == "MarioJumpState";
                    packet << 0 << obj->getPosition().x << obj->getPosition().y << obj->getSpeed().x << obj->getSpeed().y << is_jump;
                } else {
                    packet << 1 << obj->getPosition().x << obj->getPosition().y << obj->getSpeed().x << obj->getSpeed().y;
                }
                packet << obj->getId();
                client->send(packet);
            }
        }
    }

    void clientUpdate(const sf::Time& deltaTime) {
        sf::Packet packet;
        sf::Socket::Status status = clientSocket.receive(packet);
        if (status == sf::Socket::Error || status == sf::Socket::Disconnected) {
            network_type = NetworkType::None;
            return;
        }
        while (status == sf::Socket::Done) {
            int type;
            packet >> type;
            if (type == 0) {
                int obj_type;
                packet >> obj_type;
                if (obj_type == 0) {
                    float x, y, s_x, s_y;
                    bool is_jump;
                    unsigned int id;
                    packet >> x >> y >> s_x >> s_y >> is_jump >> id;
                    std::shared_ptr<Mario> player = std::make_shared<Mario>(x, y);
                    player->setId(id);
                    const auto& move_component = player->getComponent<MoveComponent>();
                    move_component->setSpeed(s_x, s_y);
                    SceneContext::getInstance().getSceneManager()->getCurrentScene()->addObjectWithMap(player);
                } else if (obj_type == 1) {
                    float x, y, s_x, s_y;
                    bool is_jump;
                    unsigned int id;
                    packet >> x >> y >> s_x >> s_y >> is_jump >> id;
                    std::shared_ptr<Mario> player = std::make_shared<Mario>(x, y, false);
                    player->setId(id);
                    const auto& move_component = player->getComponent<MoveComponent>();
                    move_component->setSpeed(s_x, s_y);
                    SceneContext::getInstance().getSceneManager()->getCurrentScene()->addObjectWithMap(player);
                } else if (obj_type == 2) {
                    float x, y, s_x, s_y, radius;
                    unsigned int id;
                    packet >> x >> y >> s_x >> s_y >> radius >> id;
                    std::shared_ptr<Player> player = std::make_shared<Player>(x, y, radius);
                    player->setId(id);
                    const auto& move_component = player->getComponent<MoveComponent>();
                    move_component->setSpeed(s_x, s_y);
                    SceneContext::getInstance().getSceneManager()->getCurrentScene()->addObjectWithMap(player);
                } else if (obj_type == 3) {
                    float x, y, s_x, s_y, w, h;
                    unsigned int id;
                    packet >> x >> y >> s_x >> s_y >> w >> h >> id;
                    std::shared_ptr<BoxGameObject> box = std::make_shared<BoxGameObject>(x, y, w, h);
                    box->setId(id);

                    const auto& move_component = box->addComponent<MoveComponent>();
                    move_component->setSpeed(s_x, s_y);
                    SceneContext::getInstance().getSceneManager()->getCurrentScene()->addObjectWithMap(box);
                }
            } else if (type == 1) {
                int obj_type;
                packet >> obj_type;
                if (obj_type == 0) {
                    float x, y, s_x, s_y;
                    bool is_jump;
                    unsigned int id;
                    packet >> x >> y >> s_x >> s_y >> is_jump >> id;
                    const std::shared_ptr<GameObject>& player = SceneContext::getInstance().getSceneManager()->getCurrentScene()->findGameObjectById(id);
                    const auto& move_component = player->getComponent<MoveComponent>();
                    move_component->setPosition(x, y);
                    move_component->setSpeed(s_x, s_y);
                    if (is_jump) player->getComponent<StateMachine>()->setState("MarioJumpState");
                } else if (obj_type == 1) {
                    float x, y, s_x, s_y;
                    unsigned int id;
                    packet >> x >> y >> s_x >> s_y >> id;
                    const std::shared_ptr<GameObject>& player = SceneContext::getInstance().getSceneManager()->getCurrentScene()->findGameObjectById(id);
                    const auto& move_component = player->getComponent<MoveComponent>();
                    move_component->setPosition(x, y);
                    move_component->setSpeed(s_x, s_y);
                }
            } else if (type == 2) {
                unsigned int id;
                packet >> id;
                SceneContext::getInstance().getSceneManager()->getCurrentScene()->removeObjectById(id);
            }

            status = clientSocket.receive(packet);
        }
    }

    void addGameObject(const std::shared_ptr<GameObject>& obj) {
        game_objects.emplace_back(obj);
    }

private:
    NetworkType network_type = NetworkType::None;
    unsigned int port = 8888;
    sf::TcpSocket clientSocket;
    sf::TcpListener listener;
    std::vector<std::shared_ptr<sf::TcpSocket>> clients;
    std::unordered_map<sf::TcpSocket*, std::shared_ptr<GameObject>> players;
    std::vector<std::shared_ptr<GameObject>> game_objects;
    long long past_time = 0;
};
