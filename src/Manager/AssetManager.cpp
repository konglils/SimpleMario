//
// Created by MINEC on 2026/5/8.
//

#include "AssetManager.h"

void AssetManager::loadTexture(const char* path) {
    if (!std::filesystem::exists(path)) {
        std::cerr << "Error: Path does not exist!" << std::endl;
        return;
    }
    if (!std::filesystem::is_directory(path)) {
        std::cerr << "Error: The provided path is not a directory!" << std::endl;
        return;
    }

    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
            std::string extension = entry.path().extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
            if (extension != ".png") continue;
            // std::cout << "loading: " << entry.path().string() << "..." << std::endl;
            const std::string file_name = entry.path().filename().string().substr(0, entry.path().filename().string().size() - 4);
            if (sf::Texture& texture = textures[file_name]; texture.loadFromFile(entry.path().string())) {
                // std::cout << file_name + " loaded successfully! size: " << texture.getSize().x << " x " << texture.getSize().y << std::endl;
            } else {
                std::cerr << "Error: unable to load " << entry.path().string() << "!" << std::endl;
            }
        }
    } catch (const std::filesystem::filesystem_error& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
}

sf::Texture& AssetManager::getTexture(const std::string& name) {
    if (textures.find(name) == textures.end()) {
        std::cerr << "Error: texture " << name << " does not exist!" << std::endl;
        return textures["default"];
    }
    return textures[name];
}

const sf::Font& AssetManager::getFont() {
    if (!have_load_font) {
        font.loadFromFile(ConfigManager::getInstance().assets.font);
        have_load_font = true;
    }
    return font;
}

void AssetManager::loadSoundBuffer(const char* path) {
    if (!std::filesystem::exists(path)) {
        std::cerr << "Error: Path does not exist!" << std::endl;
        return;
    }
    if (!std::filesystem::is_directory(path)) {
        std::cerr << "Error: The provided path is not a directory!" << std::endl;
        return;
    }

    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
            std::string extension = entry.path().extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
            if (extension != ".ogg") continue;
            // std::cout << "loading: " << entry.path().string() << "..." << std::endl;
            const std::string file_name = entry.path().filename().string().substr(0, entry.path().filename().string().size() - 4);
            if (sf::SoundBuffer& sound_buffer = soundBuffers[file_name]; sound_buffer.loadFromFile(entry.path().string())) {
                // std::cout << file_name + " loaded successfully! size: " << texture.getSize().x << " x " << texture.getSize().y << std::endl;
            } else {
                std::cerr << "Error: unable to load " << entry.path().string() << "!" << std::endl;
            }
        }
    } catch (const std::filesystem::filesystem_error& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
}

sf::SoundBuffer& AssetManager::getSoundBuffer(const std::string& name) {
    if (soundBuffers.find(name) == soundBuffers.end()) {
        std::cerr << "Error: soundBuffer " << name << " does not exist!" << std::endl;
        return soundBuffers["default"];
    }
    return soundBuffers[name];
}
