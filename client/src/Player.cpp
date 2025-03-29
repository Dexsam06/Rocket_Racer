#include "../include/Player.hpp"

#include <iostream>

Player::Player(std::unique_ptr<Collider> collider, SDL_Texture *texture, Vector2D pos, Vector2D vel, double mass, enet_uint32 ID, std::string username)
    : rotationSpeed(0), Entity(std::move(collider), texture, pos, vel, mass, ID), username(username)
{
}

void Player::update(double &xGravityForce, double &yGravityForce, double &deltaTime)
{
    thrustForce = isThrusting ? 100000 * deltaTime : 0; 
    rotationSpeed -= isTurningLeft ? 3 * deltaTime : 0;
    rotationSpeed += isTurningRight ? 3 * deltaTime : 0;

    rotation += Physics::rotation(rotationSpeed, deltaTime);

    double xThrustForce = Physics::forceVectorXAxis(thrustForce, (rotation * M_PI) / 180);
    double yThrustForce = -Physics::forceVectorYAxis(thrustForce, (rotation * M_PI) / 180);

    double xTotalForce = xGravityForce + xThrustForce;
    double yTotalForce = yGravityForce + yThrustForce;

    acceleration.x = Physics::acceleration(xTotalForce, mass);
    acceleration.y = Physics::acceleration(yTotalForce, mass);

    position.x += Physics::distance(velocity.x, acceleration.x, deltaTime);
    position.y += Physics::distance(velocity.y, acceleration.y, deltaTime);

    velocity.x += Physics::velocity(acceleration.x, deltaTime);
    velocity.y += Physics::velocity(acceleration.y, deltaTime);

    getCollider()->updatePosition(getPosition());
    getCollider()->updateRotation((rotation * M_PI) / 180); 
}

void Player::applyInput(SDL_Keycode keyCode, bool pressed)
{
    switch (keyCode)
    {
    // Left arrow
    case SDLK_LEFT:
        isTurningLeft = pressed ? true : false;
        break;
    // Right arrow
    case SDLK_RIGHT:
        isTurningRight = pressed ? true : false;
        break;
    // Spacebar
    case SDLK_SPACE:
        isThrusting = pressed ? true : false;
        break;
    }
}

void Player::draw(SDL_Renderer *renderer, int screenWidth, int screenHeight, Vector2D playerClientPos, Vector2D scalingFactor)
{
    Vector2D screenCenter(screenWidth / 2, screenHeight / 2);

    Vector2D offsetFromClientPlayer(
        position.x - playerClientPos.x,
        position.y - playerClientPos.y);

    Vector2D scaledOffset(
        offsetFromClientPlayer.x * scalingFactor.x,
        offsetFromClientPlayer.y * scalingFactor.y);

    Vector2D scaledPosition(
        screenCenter.x + scaledOffset.x,
        screenCenter.y + scaledOffset.y);

    int scaledWidth = static_cast<int>(playerWidth * scalingFactor.x);
    int scaledHeight = static_cast<int>(playerHeight * scalingFactor.y);

    SDL_Rect playerDestRect = {
        scaledPosition.x - scaledWidth / 2,
        scaledPosition.y - scaledHeight / 2,
        scaledWidth,
        scaledHeight};

    SDL_RenderCopyEx(renderer, texture, nullptr, &playerDestRect, rotation, nullptr, SDL_FLIP_NONE);
}

void Player::reconcileClientState(GameStatePacket &gameStatePacket, double lerpFactor)
{
    double serverPosX = gameStatePacket.clientState.serverPosX;
    double serverPosY = gameStatePacket.clientState.serverPosY;
    double serverRotation = gameStatePacket.clientState.rotation;

    double deltaX = serverPosX - position.x; 
    double deltaY = serverPosY - position.y;
    double distanceSquared = (deltaX * deltaX) + (deltaY * deltaY);

    double snapThreshold = 3;

    // Check if we should snap (if within the threshold)
    if (distanceSquared < snapThreshold * snapThreshold)
    {
        position.x = serverPosX;
        position.y = serverPosY;
    }
    else
    {
        // Smoothly interpolate towards the correct position
        position.x = Physics::lerp(position.x, serverPosX, lerpFactor);
        position.y = Physics::lerp(position.y, serverPosY, lerpFactor);
    }

    if (rotation > serverRotation - 5 && rotation < serverRotation + 5)
    {
        rotation = serverRotation;
    }
    else
    {
        rotation = Physics::lerp(rotation, serverRotation, lerpFactor);
    }

    isThrusting = gameStatePacket.clientState.isThrusting;
    isTurningLeft = gameStatePacket.clientState.isTurningLeft;
    isTurningRight = gameStatePacket.clientState.isTurningRight;
    rotationSpeed = 0.0; 
}
