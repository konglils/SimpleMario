//
// Created by MINEC on 2026/1/29.
//

#pragma once
#include <filesystem>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "ConfigManager.h"

class AssetManager {
public:
    static AssetManager& getInstance() {
        static AssetManager instance;
        return instance;
    }

    void loadTexture(const char* path);

    sf::Texture& getTexture(const std::string& name);

    void addTexture(const std::string& name, const sf::Texture& texture) {
        textures[name] = texture;
    }

    const sf::Font& getFont();

    void loadSoundBuffer(const char* path);

    void addSoundBuffer(const std::string& name, const sf::SoundBuffer& sound_buffer) {
        soundBuffers[name] = sound_buffer;
    }

    sf::SoundBuffer& getSoundBuffer(const std::string& name);

private:
    AssetManager() = default;
    std::unordered_map<std::string, sf::Texture> textures{};
    std::unordered_map<std::string, sf::SoundBuffer> soundBuffers{};
    sf::Font font{};
    bool have_load_font = false;
};