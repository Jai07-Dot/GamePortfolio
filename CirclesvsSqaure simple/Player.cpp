//By JairaSettles
#include "Player.h"
#include <SFML/Window/Keyboard.hpp>

Player::Player() : onGround(false), jumpsUsed(0) {
    shape.setSize(sf::Vector2f(32.f, 32.f));
    shape.setFillColor(sf::Color::Cyan);
    shape.setPosition(100.f, 400.f);
    velocity = sf::Vector2f(0.f, 0.f);
}

void Player::handleInput(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
        if (jumpsUsed < MAX_JUMPS) {
            velocity.y = JUMP_SPEED;
            jumpsUsed++;
            onGround = false;
        }
    }
}

void Player::update(float deltaTime, const std::vector<sf::RectangleShape>& collisionPool) {
    // Horizontal movement: A/D or Left/Right
    velocity.x = 0.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        velocity.x = -MOVE_SPEED;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        velocity.x = MOVE_SPEED;
    }

    // Gravity
    velocity.y += GRAVITY * deltaTime;

    // Move horizontally, then resolve horizontal collisions
    shape.move(velocity.x * deltaTime, 0.f);
    sf::FloatRect bounds = shape.getGlobalBounds();
    for (const auto& plat : collisionPool) {
        sf::FloatRect platBounds = plat.getGlobalBounds();
        if (bounds.intersects(platBounds)) {
            if (velocity.x > 0.f) {
                shape.setPosition(platBounds.left - bounds.width, shape.getPosition().y);
            }
            else if (velocity.x < 0.f) {
                shape.setPosition(platBounds.left + platBounds.width, shape.getPosition().y);
            }
            bounds = shape.getGlobalBounds();
        }
    }

    // Move vertically, then resolve vertical collisions
    shape.move(0.f, velocity.y * deltaTime);
    bounds = shape.getGlobalBounds();
    onGround = false;
    for (const auto& plat : collisionPool) {
        sf::FloatRect platBounds = plat.getGlobalBounds();
        if (bounds.intersects(platBounds)) {
            if (velocity.y > 0.f) {
                shape.setPosition(shape.getPosition().x, platBounds.top - bounds.height);
                velocity.y = 0.f;
                onGround = true;
                jumpsUsed = 0;
            }
            else if (velocity.y < 0.f) {
                shape.setPosition(shape.getPosition().x, platBounds.top + platBounds.height);
                velocity.y = 0.f;
            }
            bounds = shape.getGlobalBounds();
        }
    }
}

void Player::resetPosition(const sf::Vector2f& pos) {
    shape.setPosition(pos);
    velocity = sf::Vector2f(0.f, 0.f);
    jumpsUsed = 0;
    onGround = false;
}

sf::Vector2f Player::getPosition() const {
    return shape.getPosition();
}

sf::FloatRect Player::getBounds() const {
    return shape.getGlobalBounds();
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(shape);
}