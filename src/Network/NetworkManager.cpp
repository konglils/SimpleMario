//
// Created by MINEC on 2026/5/8.
//

#include "NetworkManager.h"

#include <iomanip>

#include "SceneContext.h"
#include "SceneManager.h"
#include "NetworkProtocol.h"

bool NetworkManager::startServer() {
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

bool NetworkManager::connectToServer(const std::string& address) {
    if (network_type == NetworkType::Client) return true;
    if (network_type == NetworkType::Server) {
        std::cout << "Cannot connect to server while already running as a server!" << std::endl;
        return false;
    }
    std::cout << "Connecting to " << address << ":" << port << "..." << std::endl;

    if (clientSocket.connect(address, port,
                             sf::seconds(ConfigManager::getInstance().network.timeout)) != sf::Socket::Done) {
        std::cout << "Failed to connect to server!" << std::endl;
        return false;
    }

    clientSocket.setBlocking(false);
    network_type = NetworkType::Client;
    std::cout << "Connected to server successfully!" << std::endl;
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
        packet << NetworkMsg::ClientInput;
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::W) {
                packet << InputType::Jump;
                clientSocket.send(packet);
            }
            else if (event.key.code == sf::Keyboard::A) {
                packet << InputType::RunLeft;
                clientSocket.send(packet);
            }
            else if (event.key.code == sf::Keyboard::D) {
                packet << InputType::RunRight;
                clientSocket.send(packet);
            }
            else if (event.key.code == sf::Keyboard::J) {
                packet << InputType::Shoot;
                clientSocket.send(packet);
            }
        }
        else if (event.type == sf::Event::KeyReleased) {
            if (event.key.code == sf::Keyboard::A) {
                packet << InputType::StopRunLeft;
                clientSocket.send(packet);
            }
            else if (event.key.code == sf::Keyboard::D) {
                packet << InputType::StopRunRight;
                clientSocket.send(packet);
            }
            else if (event.key.code == sf::Keyboard::W) {
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
        // 创建新加入的玩家
        const auto current_scene = SceneContext::getInstance().getSceneManager()->getCurrentScene();
        const auto newPlayer = current_scene->spawnEntity();
        players[newClient.get()] = std::dynamic_pointer_cast<ISerializable>(newPlayer);

        sf::Packet packet;
        // 发送新玩家信息给所有玩家
        if (!clients.empty()) {
            players[newClient.get()].lock()->serialize(packet, NetworkMsg::SpawnObject);

            for (const auto& client : clients) {
                client->send(packet);
            }
        }

        // 发送新玩家信息给新玩家自己
        packet.clear();
        players[newClient.get()].lock()->serialize(packet, NetworkMsg::SpawnPlayer);

        newClient->send(packet);

        clients.emplace_back(newClient);

        // 获取当前时间
        const std::time_t now = std::time(nullptr);
        const std::tm* local_tm = std::localtime(&now);

        std::cout << '[' << std::put_time(local_tm, "%Y-%m-%d %H:%M:%S") << "] New client connected! ";
        std::cout << "Total number of players: " << clients.size() + 1u << std::endl;
    }
}

void NetworkManager::serverUpdate(const sf::Time& deltaTime) {
    // 接收新用户连接
    receiveNewConnection();
    // 处理客户端数据
    std::unordered_map<unsigned int, bool> removeIdsMap; // 存储需要删除的玩家id
    for (auto it = clients.begin(); it != clients.end();) {
        const auto& client = *it;
        sf::Packet packet;
        sf::Socket::Status status = client->receive(packet);
        // 客户端断开连接处理
        if (status == sf::Socket::Error || status == sf::Socket::Disconnected) {
            if (const auto player = players[client.get()].lock()) {
                const unsigned int id = player->getNetworkId();
                removeIdsMap[id] = true;
                players.erase(client.get());
                player->disconnect();
                it = clients.erase(it);
                continue;
            }
            std::cerr << "NetworkManager::serverUpdate: client game object is unexpectedly released" << std::endl;
            continue;
        }
        // 处理玩家的输入操作
        while (status == sf::Socket::Done) {
            const auto player = players[client.get()].lock();
            if (!player) {
                std::cerr << "NetworkManager::serverUpdate: client game object is unexpectedly released" << std::endl;
                break;
            }
            player->deserialize(packet);
            status = client->receive(packet);
        }
        ++it;
    }
    // 在 game_objects 中删除已销毁的对象
    game_objects.erase(
        std::remove_if(game_objects.begin(), game_objects.end(), [](const auto& obj) -> bool {
            return obj.expired();
        }),
        game_objects.end()
    );

    // 通知所有在线玩家删除不在线的玩家
    for (const auto& client : clients) {
        for (const auto& [id, remove] : removeIdsMap) {
            sf::Packet packet;
            packet << NetworkMsg::RemoveObject << id;
            client->send(packet);
        }
    }

    // 向客户端同步数据
    past_time += deltaTime.asMilliseconds();
    if (past_time < ConfigManager::getInstance().network.tickRate) return;
    past_time = 0;
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
        std::cout << "NetworkManager::clientUpdate: Server disconnected" << std::endl;
        return;
    }
    while (status == sf::Socket::Done) {
        NetworkMsg type;
        packet >> type;
        if (type == NetworkMsg::SpawnObject || type == NetworkMsg::SpawnPlayer) {
            players[&clientSocket] = std::dynamic_pointer_cast<ISerializable>(
                SceneContext::getInstance().getSceneManager()->getCurrentScene()->spawnEntity(packet));
        }
        else if (type == NetworkMsg::UpdateObject) {
            unsigned int id;
            packet >> id;
            const std::shared_ptr<ISerializable>& obj = std::dynamic_pointer_cast<ISerializable>(
                SceneContext::getInstance().getSceneManager()->
                                            getCurrentScene()->findGameObjectById(id));
            if (!obj) {
                std::cerr << "NetworkManager::clientUpdate: Objects with ID " << id << " are not found" << std::endl;
                continue;
            }
            obj->deserialize(packet);
        }
        else if (type == NetworkMsg::RemoveObject) {
            unsigned int id;
            packet >> id;
            SceneContext::getInstance().getSceneManager()->
                                        getCurrentScene()->findGameObjectById(id)->destroy();
            SceneContext::getInstance().getSceneManager()->getCurrentScene()->removeObjectById(id);
        }
        else if (type == NetworkMsg::SpawnFireBall) {
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