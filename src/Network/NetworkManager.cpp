//
// Created by MINEC on 2026/5/8.
//

#include "NetworkManager.h"

#include <iomanip>
#include <ranges>

#include "Logger.h"
#include "SceneContext.h"
#include "SceneManager.h"
#include "NetworkProtocol.h"

bool NetworkManager::startServer() {
    if (network_type == NetworkType::Server) return true;
    if (network_type == NetworkType::Client) {
        LOG_INFO("Cannot start server while already running as a client!");
        return false;
    }
    LOG_INFO_FMT("Starting server on port {} ...", port);

    if (listener.listen(port) != sf::Socket::Done) {
        LOG_WARN("Failed to start server");
        return false;
    }

    listener.setBlocking(false);
    network_type = NetworkType::Server;
    LOG_INFO("Server started successfully!");
    return true;
}

bool NetworkManager::connectToServer(const std::string& address) {
    if (network_type == NetworkType::Client) return true;
    if (network_type == NetworkType::Server) {
        LOG_WARN("Cannot connect to server while already running as a server!");
        return false;
    }
    LOG_INFO_FMT("Connecting to server at {}:{}", address, port);

    if (clientSocket.connect(address, port,
                             sf::seconds(CONFIG.network.timeout)) != sf::Socket::Done) {
        LOG_WARN("Failed to connect to server!");
        return false;
    }

    clientSocket.setBlocking(false);
    network_type = NetworkType::Client;
    LOG_INFO("Connected to server successfully!");
    return true;
}

void NetworkManager::update(const sf::Time& deltaTime) {
    if (network_type == NetworkType::None) return;
    if (network_type == NetworkType::Server) {
        serverUpdate(deltaTime);
    }
    else {
        clientUpdate(deltaTime);
    }
}

void NetworkManager::handleEvent(const sf::Event& event) {
    if (network_type != NetworkType::None && event.type == sf::Event::Closed) {
        if (network_type == NetworkType::Server) {
            for (const auto& client : clients) {
                client->disconnect();
            }
            clients.clear();
        }
        else {
            clientSocket.disconnect();
        }
    }
    if (network_type == NetworkType::Client) {
        sf::Packet packet;
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::W) {
                packet << NetworkMsg::ClientInput << NetworkMsg::ClientInput;
                packet << InputType::Jump;
                clientSocket.send(packet);
            }
            else if (event.key.code == sf::Keyboard::A) {
                packet << NetworkMsg::ClientInput << NetworkMsg::ClientInput;
                packet << InputType::RunLeft;
                clientSocket.send(packet);
            }
            else if (event.key.code == sf::Keyboard::D) {
                packet << NetworkMsg::ClientInput << NetworkMsg::ClientInput;
                packet << InputType::RunRight;
                clientSocket.send(packet);
            }
            else if (event.key.code == sf::Keyboard::J) {
                packet << NetworkMsg::ClientInput << NetworkMsg::ClientInput;
                packet << InputType::Shoot;
                clientSocket.send(packet);
            }
            else if (event.key.code == sf::Keyboard::R) {
                packet << NetworkMsg::ClientRespawn;
                clientSocket.send(packet);
            }
        }
        else if (event.type == sf::Event::KeyReleased) {
            if (event.key.code == sf::Keyboard::A) {
                packet << NetworkMsg::ClientInput << NetworkMsg::ClientInput;
                packet << InputType::StopRunLeft;
                clientSocket.send(packet);
            }
            else if (event.key.code == sf::Keyboard::D) {
                packet << NetworkMsg::ClientInput << NetworkMsg::ClientInput;
                packet << InputType::StopRunRight;
                clientSocket.send(packet);
            }
            else if (event.key.code == sf::Keyboard::W) {
                packet << NetworkMsg::ClientInput << NetworkMsg::ClientInput;
                packet << InputType::JumpRelease;
                clientSocket.send(packet);
            }
        }
    }
}

void NetworkManager::receiveNewConnection() {
    if (const auto newClient = std::make_shared<sf::TcpSocket>(); listener.accept(*newClient) == sf::Socket::Done) {
        newClient->setBlocking(false);
        // 给客户端发送当前场景信息
        for (auto it = game_objects.begin(); it != game_objects.end();) {
            if (const auto obj = it->lock()) {
                sf::Packet packet;
                obj->serialize(packet, NetworkMsg::SpawnObject);
                newClient->send(packet);
                ++it;
            } else {
                it = game_objects.erase(it);
            }
        }

        createNewPlayer(newClient);

#ifndef SERVER_BUILD
        LOG_INFO_FMT("New client connected! Total number of players: {}", clients.size() + 1u);
#else
        LOG_INFO_FMT("New client connected! Total number of players: {}", clients.size());
#endif
    }
}

void NetworkManager::createNewPlayer(const std::shared_ptr<sf::TcpSocket> newClient) {
    // 创建新加入的玩家
    const auto current_scene = SceneContext::getInstance().getSceneManager()->getCurrentScene();
    // 注意，这里的spawnEntity方法会广播创建玩家，所以下面那一段可以注释了。
    // 但是有可能 client 已经在 clients vector 里导致重复生成，所以先删了。
    // 好蠢的方法，但是先写完没 bug 再说 ಥ_ಥ
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        if (*it == newClient) {
            clients.erase(it);
            break;
        }
    }
    const auto newPlayer = current_scene->spawnEntity();
    players[newClient.get()] = std::dynamic_pointer_cast<ISerializable>(newPlayer);

    sf::Packet packet;
    // 发送新玩家信息给所有玩家
    // if (!clients.empty()) {
    //     players[newClient.get()].lock()->serialize(packet, NetworkMsg::SpawnObject);
    //
    //     for (const auto& client : clients) {
    //         if (client != newClient) client->send(packet);
    //     }
    // }

    // 发送新玩家信息给新玩家自己
    players[newClient.get()].lock()->serialize(packet, NetworkMsg::SpawnPlayer);

    newClient->send(packet);

    clients.emplace_back(newClient);
}

void NetworkManager::serverUpdate(const sf::Time& deltaTime) {
    // 接收新用户连接
    receiveNewConnection();
    // 处理客户端数据
    std::unordered_map<unsigned int, bool> removeIdsMap; // 存储需要删除的玩家id
    for (auto it = clients.begin(); it != clients.end();) {
        const auto& client = *it;

        const auto player = players[client.get()].lock();
        // 如果玩家死亡会导致这个弱指针失效
        if (!player) {
            LOG_TRACE("client player died and weak ptr is invalid");
            players.erase(client.get());
        }

        sf::Packet packet;
        sf::Socket::Status status = client->receive(packet);
        // 客户端断开连接处理
        if (status == sf::Socket::Error || status == sf::Socket::Disconnected) {
            if (player) {
                const unsigned int id = player->getNetworkId();
                removeIdsMap[id] = true;
                players.erase(client.get());
                player->disconnect();
                it = clients.erase(it);
                LOG_DEBUG("client removed in clients vector");
                continue;
            }
            // 失效了也需要移除，因为client断开了
            it = clients.erase(it);
            LOG_INFO("client game object is released");
            continue;
        }
        // 处理玩家的输入操作
        while (status == sf::Socket::Done) {
            NetworkMsg msg_type;
            packet >> msg_type;
            if (msg_type == NetworkMsg::ClientRespawn) {
                LOG_INFO("client request to respawn");
                createNewPlayer(client);
            } else if (msg_type == NetworkMsg::ClientInput) {
                if (player) {
                    player->deserialize(packet);
                }
            }
            status = client->receive(packet);
        }
        ++it;
    }
    // 在 game_objects 中删除已销毁的对象
    std::erase_if(game_objects, [](const auto& obj) -> bool {
        return obj.expired();
    });

    // 通知所有在线玩家删除不在线的玩家
    for (const auto& client : clients) {
        for (const auto& id : removeIdsMap | std::views::keys) {
            sf::Packet packet;
            packet << NetworkMsg::RemoveObject << id;
            client->send(packet);
        }
    }

    // 向客户端同步数据
    past_time += deltaTime.asMilliseconds();
    if (past_time < 1000 / CONFIG.network.tickRate) return;
    past_time = 0;
    LOG_TRACE("Sending update packets to clients");
    for (const auto& client : clients) {
        for (const auto& obj : game_objects) {
            sf::Packet packet;
            obj.lock()->serialize(packet, NetworkMsg::UpdateObject);
            if (packet.getDataSize() == 0) {
                continue;
            }
            client->send(packet);
        }
    }
}

void NetworkManager::clientUpdate(const sf::Time& deltaTime) {
    sf::Packet packet;
    sf::Socket::Status status = clientSocket.receive(packet);
    if (status == sf::Socket::Error || status == sf::Socket::Disconnected) {
        network_type = NetworkType::None;
        LOG_WARN("Server disconnected");
        return;
    }
    while (status == sf::Socket::Done) {
        NetworkMsg type;
        packet >> type;
        if (type == NetworkMsg::SpawnObject || type == NetworkMsg::SpawnPlayer) {
            LOG_TRACE("Received packet, type: SpawnObject or SpawnPlayer");
            players[&clientSocket] = std::dynamic_pointer_cast<ISerializable>(
                SceneContext::getInstance().getSceneManager()->getCurrentScene()->spawnEntity(packet));
        }
        else if (type == NetworkMsg::UpdateObject) {
            LOG_TRACE("Received packet, type: UpdateObject");
            unsigned int id;
            packet >> id;
            const std::shared_ptr<ISerializable>& obj = std::dynamic_pointer_cast<ISerializable>(
                SceneContext::getInstance().getSceneManager()->
                                            getCurrentScene()->findGameObjectById(id));
            if (!obj) {
                LOG_ERROR_FMT("Object with ID {} are not found", id);
                packet.clear();
                continue;
            }
            obj->deserialize(packet);
        }
        else if (type == NetworkMsg::RemoveObject) {
            LOG_TRACE("Received packet, type: RemoveObject");
            unsigned int id;
            packet >> id;
            SceneContext::getInstance().getSceneManager()->
                                        getCurrentScene()->findGameObjectById(id)->destroy();
            SceneContext::getInstance().getSceneManager()->getCurrentScene()->removeObjectById(id);
        }
        else if (type == NetworkMsg::SpawnFireBall) {
            LOG_TRACE("Received packet, type: SpawnFireBall");
            SceneContext::getInstance().getSceneManager()->getCurrentScene()->spawnEntity(packet);
        }
        status = clientSocket.receive(packet);
    }
}

void NetworkManager::addGameObject(const std::shared_ptr<GameObject>& obj) {
    const auto& serializable_obj = std::dynamic_pointer_cast<ISerializable>(obj);
    game_objects.emplace_back(serializable_obj);
    if (this->network_type == NetworkType::Server) {
        // 向所有客户端广播新对象
        for (const auto& client : clients) {
            sf::Packet spawn_packet;
            serializable_obj->serialize(spawn_packet, NetworkMsg::SpawnObject);
            client->send(spawn_packet);
        }
    }
}

void NetworkManager::broadcast(sf::Packet& packet) const {
    for (const auto& client : clients) {
        client->send(packet);
    }
}
