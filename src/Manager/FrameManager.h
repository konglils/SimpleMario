//
// Created by MINEC on 2026/1/29.
//

#pragma once
#include <string>
#include <unordered_map>
#include <Animation.h>
#include <fstream>
#include <iostream>

#include "AssetManager.h"
#include <nlohmann/json_fwd.hpp>
using json = nlohmann::json;

class FrameManager {
public:
    static FrameManager& getInstance() {
        static FrameManager instance;
        return instance;
    }

    std::vector<Animation::Frame>* getFrame(const std::string& name) {
        if (frames.find(name) == frames.end()) {
            std::cerr << "Error: animation " << name << " does not exist!" << std::endl;
            return nullptr;
        }
        return &frames[name];
    }

    void loadFrame() {
        loadFrameFromJson("./Asset/SuperMario/source/data/player/mario.json");
        loadFrameFromJson("./Asset/SuperMario/source/data/player/box.json");
    }

    void loadFrameFromJson(const char* path);

private:
    FrameManager() = default;
    std::unordered_map<std::string, std::vector<Animation::Frame>> frames{};
};
