//
// Created by MINEC on 2026/5/8.
//

#pragma once
#include <memory>
#include "ISerializable.h"
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include "ConfigManager.h"
#include "GameObject.h"

class NetworkManager {
public:
    enum class NetworkType : uint8_t {
        None,
        Server,
        Client
    };
    NetworkManager() = default;
    ~NetworkManager() = default;

    NetworkType getNetworkType() const {
        return network_type;
    }

    bool startServer();

    bool connectToServer(const std::string& address);

    void update(const sf::Time& deltaTime);

    void handleEvent(const sf::Event& event);

    void serverUpdate(const sf::Time& deltaTime);

    void receiveNewConnection();

    void clientUpdate(const sf::Time& deltaTime);

    void addGameObject(const std::shared_ptr<GameObject>& obj);

    void broadcast(sf::Packet& packet) const;

    void createNewPlayer(const std::shared_ptr<sf::TcpSocket> newClient);

private:
    NetworkType network_type = NetworkType::None;
    unsigned int port = CONFIG.network.port;
    sf::TcpSocket clientSocket;
    sf::TcpListener listener;
    std::vector<std::shared_ptr<sf::TcpSocket>> clients;
    std::unordered_map<sf::TcpSocket*, std::weak_ptr<ISerializable>> players;
    // 需要同步的游戏对象
    std::vector<std::weak_ptr<ISerializable>> game_objects;
    int past_time = 0;
};
