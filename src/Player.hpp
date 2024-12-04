#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <vector>
#include "Entity.hpp"
#include <memory>

class Player : public Entity
{
public:
    // Constructor
    Player(std::unique_ptr<Collider> collider, SDL_Texture *texture, Vector2D pos, Vector2D vel, double mass);
    ~Player(){};
    void setThrust(double thrust) { this->thrust = thrust; }
    void setPlayerWidth(int playerWidth) { this->playerWidth = playerWidth; }
    void setPlayerHeight(int playerHeight) { this->playerHeight = playerHeight; }
    void setRotationSpeed(double rotationSpeed) { this->rotationSpeed += rotationSpeed; }
    void setPosition(Vector2D position) { this->position = position; }
    void setVelocity(Vector2D velocity) { this->velocity = velocity; }
    void setRotation(double rotation) { this->rotation = rotation; }

    double getRotationSpeed() const { return rotationSpeed; }
    
    std::unique_ptr<Entity> clone() const override
    {
        return std::make_unique<Player>(
            collider->clone(), texture, position, velocity, mass
        );
    }

private:
    double thrust;
    double rotationSpeed, rotation;
    int playerWidth, playerHeight;

    void update(double &xGravityForce, double &yGravityForce, double &deltaTime) override;
    void draw(SDL_Renderer *renderer, int screenWidth, int screenHeight, Vector2D playerPos, Vector2D scalingFactor) override;
};

#endif
