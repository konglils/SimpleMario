//
// Created by MINEC on 2026/1/29.
//

#pragma once
#include <functional>
#include <SFML/Graphics.hpp>

class Timer {
public:
    Timer() = default;
    ~Timer() = default;

    void start(const int _aim_time, const bool _is_loop = false) {
        this->aim_time = _aim_time;
        this->started = true;
        this->is_loop = _is_loop;
        this->reset();
    }

    void update(const sf::Time& deltaTime) {
        if (started) {
            past_time += deltaTime.asMilliseconds();
            if (past_time >= aim_time) {
                if (callback) callback();
                if (is_loop) {
                    reset();
                } else {
                    stop();
                }
            }
        }
    }

    void setCallback(const std::function<void()>& _callback) {
        this->callback = _callback;
    }

    void reset() {
        past_time = 0;
    }

    void stop() {
        started = false;
    }

private:
    int past_time = 0;
    int aim_time = 0;
    bool started = false;
    bool is_loop = false;
    std::function<void()> callback;
};
