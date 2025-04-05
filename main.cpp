#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <chrono>
#include <sstream>
#include "ball.hpp"

const float GRAVITY = 500.0f;
const float BOUNCE = 0.8f;
const float FRICTION = 0.999f;
const float BALL_RADIUS = 15.0f;
const float BALL_SPAWN_INTERVAL = 0.1f;
const float INITIAL_SPEED = 300.0f;
const unsigned int MAX_BALLS = 100;
const float LAUNCH_ANGLE = 45.0f;
const float HUE_INCREMENT = 10.0f;

sf::Color hsvToRgb(float h, float s, float v) {
     h = std::fmod(h, 360.0f);
     float c = v * s;
     float x = c * (1 - std::abs(std::fmod(h / 60.0f, 2) - 1));
     float m = v - c;

     float r, g, b;
     if (h < 60) { r = c; g = x; b = 0; }
     else if (h < 120) { r = x; g = c; b = 0; }
     else if (h < 180) { r = 0; g = c; b = x; }
     else if (h < 240) { r = 0; g = x; b = c; }
     else if (h < 300) { r = x; g = 0; b = c; }
     else { r = c; g = 0; b = x; }

     return sf::Color(static_cast<sf::Uint8>((r + m) * 255), 
     static_cast<sf::Uint8>((g + m) * 255), 
     static_cast<sf::Uint8>((b + m) * 255));
}

int main() {
     sf::RenderWindow window(sf::VideoMode(800, 600), "Verlet Ball Simulation");
     window.setFramerateLimit(60);

     sf::Font font;
     if (!font.loadFromFile("font.ttf")) {
          return EXIT_FAILURE;
     }

     sf::Text infoText;
     infoText.setFont(font);
     infoText.setCharacterSize(16);
     infoText.setFillColor(sf::Color::White);
     infoText.setPosition(10, 10);

     std::vector<Ball> balls;
     float spawnTimer = 0.0f;
     float hue = 0.0f;

     auto lastTime = std::chrono::high_resolution_clock::now();

     while (window.isOpen()) {
          auto now = std::chrono::high_resolution_clock::now();
          float dt = std::chrono::duration<float>(now - lastTime).count();
          lastTime = now;
          float fps = 1.0f / dt;

          sf::Event event;
          while (window.pollEvent(event)) {
               if (event.type == sf::Event::Closed) {
                    window.close();
               }
               if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                    window.close();
               }
          }

          spawnTimer += dt;
          if (spawnTimer >= BALL_SPAWN_INTERVAL && balls.size() < MAX_BALLS) {
               spawnTimer = 0.0f;

               sf::Color color = hsvToRgb(hue, 1.0f, 1.0f);
               hue = std::fmod(hue + HUE_INCREMENT, 360.0f);

               Ball newBall(50.0f, 50.0f, BALL_RADIUS, color);

               float angle = LAUNCH_ANGLE * 3.14159f / 180.0f;
               sf::Vector2f direction(std::cos(angle), std::sin(angle));
               newBall.setVelocity(direction * INITIAL_SPEED, dt);

               balls.push_back(newBall);
          }

          for (auto& ball : balls) {
               ball.accelerate(sf::Vector2f(0, GRAVITY));

               ball.update(dt);

               sf::Vector2f velocity = ball.getVelocity(dt) * FRICTION;
               ball.setVelocity(velocity, dt);

               sf::Vector2f pos = ball.position;
               float r = ball.radius;

               if (pos.x - r < 0) {
                    ball.position.x = r;
                    velocity.x = -velocity.x * BOUNCE;
                    ball.setVelocity(velocity, dt);
               } else if (pos.x + r > window.getSize().x) {
                    ball.position.x = window.getSize().x - r;
                    velocity.x = -velocity.x * BOUNCE;
                    ball.setVelocity(velocity, dt);
               }

               if (pos.y - r < 0) {
                    ball.position.y = r;
                    velocity.y = -velocity.y * BOUNCE;
                    ball.setVelocity(velocity, dt);
               } else if (pos.y + r > window.getSize().y) {
                    ball.position.y = window.getSize().y - r;
                    velocity.y = -velocity.y * BOUNCE;
                    ball.setVelocity(velocity, dt);
               }
          }

          for (size_t i = 0; i < balls.size(); ++i) {
               for (size_t j = i + 1; j < balls.size(); ++j) {
                    sf::Vector2f delta = balls[j].position - balls[i].position;
                    float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);
                    float minDist = balls[i].radius + balls[j].radius;

                    if (dist < minDist && dist > 0.0f) {
                         sf::Vector2f normal = delta / dist;
                         float overlap = minDist - dist;

                         balls[i].position -= normal * overlap * 0.5f;
                         balls[j].position += normal * overlap * 0.5f;

                         sf::Vector2f relVel = balls[j].getVelocity(dt) - balls[i].getVelocity(dt);
                         float velAlongNormal = relVel.x * normal.x + relVel.y * normal.y;

                         if (velAlongNormal > 0) continue;

                         float impulse = -(1.0f + BOUNCE) * velAlongNormal;
                         impulse /= 2.0f;

                         sf::Vector2f impulseVec = normal * impulse;
                         balls[i].setVelocity(balls[i].getVelocity(dt) - impulseVec, dt);
                         balls[j].setVelocity(balls[j].getVelocity(dt) + impulseVec, dt);
                    }
               }
          }

          std::ostringstream ss;
          ss << "FPS: " << static_cast<int>(fps) << "\n";
          ss << "Frame Time: " << dt * 1000.0f << " ms\n";
          ss << "Balls: " << balls.size() << " / " << MAX_BALLS;
          infoText.setString(ss.str());

          window.clear(sf::Color(30, 30, 40));

          for (const auto& ball : balls) {
               ball.draw(window);
          }

          window.draw(infoText);
          window.display();
     }

     return 0;
}