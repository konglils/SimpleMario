//
// Created by MINEC on 2026/5/8.
//

#pragma once
#include <SFML/Network.hpp>
#include "NetworkProtocol.h"

class ISerializable {
public:
    virtual ~ISerializable() = default;
    virtual void serialize(sf::Packet& packet, NetworkMsg type) = 0;
    virtual void deserialize(sf::Packet& packet) = 0;
    virtual unsigned int getNetworkId() = 0;
    virtual void disconnect() = 0;
};