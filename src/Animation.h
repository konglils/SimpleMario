//
// Created by MINEC on 2026/1/29.
//

#pragma once
#include <vector>
#include <SFML/Graphics.hpp>


class Animation {
public:
    struct Frame {
        sf::Texture* texture;
        sf::IntRect textureRect;
        sf::Vector2f origin = {0.f, 0.f};
        sf::Vector2f scale = {1.f, 1.f};
        unsigned int duration = 100;
    };
    Animation() = default;
    ~Animation() = default;

    void addFrame(const Frame& frame) const {
        frames->push_back(frame);
    }

    void setBack(const bool flag) {
        this->back = flag;
    }

    void setFrames(std::vector<Frame>* _frames) {
        this->frames = _frames;
    }

    void update(const sf::Time& deltaTime) {
        currentFrameDuration += deltaTime.asMilliseconds();
        if (currentFrameDuration >= (*frames)[currentFrame].duration) {
            currentFrameDuration = 0;
            if (back) {
                if (currentFrame == 0) add = 1;
                else if (currentFrame == frames->size() - 1) add = -1;
                currentFrame = currentFrame + add;
            } else {
                currentFrame = (currentFrame + 1) % frames->size();
            }
        }
    }

    void render(sf::RenderWindow* window, const sf::Vector2f& position) {
        sf::Sprite& sprite_ = this->getSprite();
        sprite_.setPosition(position);
        window->draw(sprite_);
    }

    Frame& getFrame() const {
        return (*frames)[currentFrame];
    }

    std::vector<Frame>& getFrames() const {
        return (*frames);
    }

    sf::Sprite& getSprite() {
        sprite.setTexture(*(*frames)[currentFrame].texture);
        sprite.setTextureRect((*frames)[currentFrame].textureRect);
        sprite.setOrigin((*frames)[currentFrame].origin);
        sprite.setScale((*frames)[currentFrame].scale);
        return sprite;
    }

private:
    unsigned int currentFrame = 0;
    unsigned int currentFrameDuration = 0;
    std::vector<Frame>* frames{};
    sf::Sprite sprite;
    bool back = false;
    int add = 1;
};
