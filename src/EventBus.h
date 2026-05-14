//
// Created by MINEC on 2025/12/9.
//


#pragma once
#include <unordered_map>
#include <string>
#include <functional>
#include <vector>
#include <any>

class EventBus {
public:
    static EventBus& getInstance() {
        static EventBus instance;
        return instance;
    }

    template <typename T>
    void subscribe(const std::string& event, std::function<void(T)> callback) {
        listeners[event].push_back([callback](std::any data) {
            callback(std::any_cast<T>(data));
        });
        LOG_TRACE_FMT("{} subscribed", event);
    }

    template <typename T>
    void publish(const std::string& eventName, T data) {
        if (listeners.contains(eventName)) {
            for (auto& listener : listeners[eventName]) {
                listener(data);
            }
        } else {
            LOG_WARN_FMT("{} not found", eventName);
        }
    }

    void removeSubscribe(const std::string& event_name) {
        if (!listeners.contains(event_name)) {
            LOG_WARN_FMT("{} not found", event_name);
            return;
        }
        LOG_TRACE_FMT("{} removed", event_name);
        listeners.erase(event_name);
    }

private:
    std::unordered_map<std::string, std::vector<std::function<void(std::any)>>> listeners;
    EventBus() = default;
};

