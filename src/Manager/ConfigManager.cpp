//
// Created by MINEC on 2026/4/4.
//

#include "ConfigManager.h"
#include <fstream>
#include <nlohmann/json.hpp>

bool ConfigManager::load() {
    try {
        const auto path = "./Asset/config.json";
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Failed to open config file: " << path << std::endl;
            return false;
        }

        json config;
        file >> config;

        parseWindow(config["window"]);
        parseAssets(config["assets"]);
        parseNetwork(config["network"]);
        parseGame(config["game"]);

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Config parse error: " << e.what() << std::endl;
        return false;
    }
}

void ConfigManager::parseWindow(const json& j) {
    window.width = j.value("width", window.width);
    window.height = j.value("height", window.height);
    window.title = j.value("title", window.title);
    window.fps = j.value("fps", window.fps);
}

void ConfigManager::parseAssets(const json& j) {
    if (j.contains("textures")) {
        for (auto& [key, value] : j["textures"].items()) {
            assets.textures[key] = value.get<std::string>();
        }
    }
    if (j.contains("sounds")) {
        for (auto& [key, value] : j["sounds"].items()) {
            assets.sounds[key] = value.get<std::string>();
        }
    }
    if (j.contains("animations")) {
        for (auto& [key, value] : j["animations"].items()) {
            assets.frames[key] = value.get<std::string>();
        }
    }
    if (j.contains("models")) {
        for (auto& [key, value] : j["models"].items()) {
            assets.models[key] = value.get<std::string>();
        }
    }
    assets.font = j.value("font", assets.font);
}

void ConfigManager::parseNetwork(const json& j) {
    network.serverIp = j.value("serverIp", network.serverIp);
    network.port = j.value("port", network.port);
    network.tickRate = j.value("tickRate", network.tickRate);
    network.timeout = j.value("timeout", network.timeout);
}

void ConfigManager::parseGame(const json& j) {
    game.gravity = j.value("gravity", game.gravity);
    game.playerSpeed = j.value("playerSpeed", game.playerSpeed);
    game.jumpForce = j.value("jumpForce", game.jumpForce);
    game.debug = j.value("debug", game.debug);
}




