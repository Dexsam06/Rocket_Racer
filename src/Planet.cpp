#include "Planet.hpp"
#include "Physics.hpp"

Planet::Planet(double radius, double xPos, double yPos, double mass, double xVelocity, double yVelocity)
    : radius(radius), Entity(xPos, yPos, mass, xVelocity, yVelocity) { 
} 



void Planet::draw (SDL_Renderer *renderer, double screenWidth, double screenHeight) 
{
   SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    // Iterate over the bounding square of the circle
    for (int y = yPos - radius; y <= yPos + radius; ++y) {
        for (int x = xPos - radius; x <= xPos + radius; ++x) {
            int dx = x - xPos;
            int dy = y - yPos;

            // Check if the point (x, y) is inside the circle
            if (dx * dx + dy * dy <= radius * radius) {
                SDL_RenderDrawPoint(renderer, x, y);  
            }
        }
    }
}