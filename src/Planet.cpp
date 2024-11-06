#include "Planet.hpp"
#include "Physics.hpp"

Planet::Planet(double radius, double xPos, double yPos, double mass, double xVelocity, double yVelocity)
    : radius(radius), Entity(xPos, yPos, mass, xVelocity, yVelocity) { 
} 

void Planet::draw(SDL_Renderer* renderer, int screenWidth, int screenHeight) {
   
    int centerX = xPos - entityXPos + screenWidth / 2;
    int centerY = yPos - entityYPos + screenHeight / 2;

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); 

    for (int y = centerY - radius; y <= centerY + radius; ++y) {
        for (int x = centerX - radius; x <= centerX + radius; ++x) {
            int dx = x - centerX;
            int dy = y - centerY;

            if (dx * dx + dy * dy <= radius * radius) { 
                SDL_RenderDrawPoint(renderer, x, y);  
            }
        }
    }
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawLine(renderer, centerX, centerY, screenWidth / 2, screenHeight / 2);
}