//By JairaSettles
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

// Implements the control scheme described in the game's own menu text:
//  - Move:   [A][D] or [Left][Right]
//  - Jump:   [Space] (double-jump allowed mid-air)
//  - Climb:  hold [W]/[Up] against a wall (main.cpp already detects "againstWall"
//            itself and nudges the player up, so this class only needs to handle
//            ground movement, gravity, jumping, and platform collision)
class Player {
public:
    Player();

    void handleInput(const sf::Event& event);
    void update(float deltaTime, const std::vector<sf::RectangleShape>& collisionPool);
    void resetPosition(const sf::Vector2f& pos);
    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const;
    void draw(sf::RenderWindow& window);

private:
    sf::RectangleShape shape;
    sf::Vector2f velocity;
    bool onGround;
    int jumpsUsed;

    static constexpr float MOVE_SPEED = 220.f;
    static constexpr float GRAVITY = 900.f;
    static constexpr float JUMP_SPEED = -420.f;
    static constexpr int MAX_JUMPS = 2;
};