#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SDL2/SDL_render.h>

class Entity {
public:
    Entity(double xPos, double yPos, double mass, double xPosVelocity, double yPosVelocity); 
    virtual ~Entity(); 
    virtual void draw(SDL_Renderer *renderer, double screenWidth, double screenHeight);
    double getXPos() {return xPos; }
    double getYPos() {return yPos; }
    double getMass() {return mass; }
protected:
    SDL_Texture* texture;
    double xPos, yPos;
    double xVelocity, yVelocity;
    double mass; 
    double xAcceleration, yAcceleration;
};

#endif 
