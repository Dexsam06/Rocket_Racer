#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SDL2/SDL_render.h>

class Entity {
public:
    Entity(double x, double y, double mass); 
    virtual ~Entity(); 

protected:
    virtual void draw(SDL_Renderer* renderer);

private:
    SDL_Texture* texture;
    double xPos, yPos;
    double xVelocity, yVelocity;
    double mass;
};

#endif 
