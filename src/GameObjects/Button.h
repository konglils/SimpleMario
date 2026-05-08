//
// Created by MINEC on 2026/2/19.
//

#pragma once
#include <functional>

#include "GameObject.h"

class Button : public GameObject {
public:
    Button( float x,  float y,  float w,  float h, const sf::String& button_text = "Button");
    void render(sf::RenderWindow* window) override;

    void handleEvent(sf::Event& event) override;

    bool isMouseOver() const;

    void setOnClick(std::function<void()>&& _onClick) {
        this->onClick = std::move(_onClick);
    }

    void setToRectCenter(const float x, const float y, const float w, const float h) {
        shape.setPosition(x + w * 0.5f - shape.getSize().x * 0.5f, y + h * 0.5f - shape.getSize().y * 0.5f);
        text.setPosition(x + (w - text.getGlobalBounds().width) * 0.5f, y + (h - text.getGlobalBounds().height) * 0.5f);
    }

    void runOnClick() const {
        if (onClick) onClick();
    }

private:
    sf::RectangleShape shape;
    std::function<void()> onClick;
    sf::Text text;
    bool is_hover = false;
};
