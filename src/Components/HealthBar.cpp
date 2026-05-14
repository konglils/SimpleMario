//
// Created by MINEC on 2026/5/14.
//

#include "HealthBar.h"
#include "GameObject.h"

HealthBar::HealthBar() {
    invulnerable_timer.setCallback([this]() { invulnerable = false; });
}

void HealthBar::update(const sf::Time& deltaTime) {
    Component::update(deltaTime);
    invulnerable_timer.update(deltaTime);
}

void HealthBar::takeDamage(const int damage) {
    if (dead) return;
    if (invulnerable) return;
    health -= damage;
    if (health <= 0) {
        health = 0;
        dead = true;
        return;
    }
    invulnerable = true;
    invulnerable_timer.start(1000);
}

bool HealthBar::isDead() const {
    return dead;
}

#ifndef SERVER_BUILD
void HealthBar::render(sf::RenderWindow* window) {
    Component::render(window);
    if (dead) return;
    // TODO: 根据 owner 改变血条形状
    float barWidth = 48.f;
    float barHeight = 5.f;
    float barX = owner->getCenter().x - barWidth / 2;
    float barY = owner->getPosition().y - 10.f;

    sf::RectangleShape bg(sf::Vector2f(barWidth, barHeight));
    bg.setFillColor(sf::Color(60, 60, 60));
    bg.setPosition(barX, barY);
    bg.setOutlineThickness(1.f);
    bg.setOutlineColor(sf::Color(0, 0, 0));
    window->draw(bg);

    float healthRatio = static_cast<float>(health) / static_cast<float>(max_health);
    sf::RectangleShape fg(sf::Vector2f(barWidth * healthRatio, barHeight));
    sf::Color fgColor;
    if (healthRatio > 0.5f) {
        fgColor = sf::Color::Green;
    } else if (healthRatio > 0.25f) {
        fgColor = sf::Color::Yellow;
    } else {
        fgColor = sf::Color::Red;
    }
    if (invulnerable) {
        fgColor = sf::Color(150, 150, 150);
    }
    fg.setFillColor(fgColor);
    fg.setPosition(barX, barY);
    window->draw(fg);
}
#endif
