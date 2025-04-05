#pragma once
#include <SFML/Graphics.hpp>

class Ball {
     public:
          sf::Vector2f position;
          sf::Vector2f position_old;
          sf::Vector2f acceleration;
          float radius;
          sf::Color color;

          Ball(float x, float y, float r, sf::Color c) 
               : position(x, y), position_old(x, y), acceleration(0, 0), radius(r), color(c) {}

          void update(float dt) {
               sf::Vector2f velocity = position - position_old;
               position_old = position;
               position += velocity + acceleration * dt * dt;
               acceleration = {};
          }

          void accelerate(sf::Vector2f force) {
               acceleration += force;
          }

          void setVelocity(sf::Vector2f v, float dt) {
               position_old = position - (v * dt);
          }

          sf::Vector2f getVelocity(float dt) const {
               return (position - position_old) / dt;
          }

          void draw(sf::RenderWindow& window) const {
               sf::CircleShape circle(radius);
               circle.setFillColor(color);
               circle.setOrigin(radius, radius);
               circle.setPosition(position);
               window.draw(circle);
          }
};