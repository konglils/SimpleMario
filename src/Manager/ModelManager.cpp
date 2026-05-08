//
// Created by MINEC on 2026/5/8.
//

#include "ModelManager.h"
#include <array>

void ModelManager::loadModel(const std::string& file_name, std::string name) {
    if (name == "default") name = "default" + std::to_string(count++);
    Model& new_model = models[name];
    std::ifstream in(file_name);
    std::string line;
    while (std::getline(in, line)) {
        if (line[0] == 'v' && line[1] == ' ') {
            std::array<float, 3> arr{};
            int cnt = 0;
            for (int i = 2; i < line.size(); i++) {
                std::string num;
                while (i < line.size() && line[i] == ' ') i++;
                while (i < line.size() && line[i] != ' ') {
                    num += line[i];
                    i++;
                }
                arr[cnt++] = std::stof(num);
            }
            new_model.points.emplace_back(arr[0], arr[1], arr[2]);
        } else if (line[0] == 'f') {
            new_model.faces.emplace_back();
            for (int i = 2; i < line.size(); i++) {
                std::string num;
                while (i < line.size() && line[i] != ' ') {
                    num += line[i];
                    i++;
                }
                int n = 0, j = 0;
                while (j < num.size() && num[j] != '/') {
                    n = 10 * n + num[j] - '0';
                    j++;
                }
                new_model.faces.back().emplace_back(n - 1);
            }
        }
    }
}
