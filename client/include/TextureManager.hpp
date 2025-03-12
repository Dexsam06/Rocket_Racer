#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <SDL2/SDL.h>
#include <string>
#include <unordered_map>
#include <SDL2/SDL_image.h>

class TextureManager {
public:
    SDL_Texture* loadTexture(const std::string& id, const std::string& filename, SDL_Renderer* renderer);
};

#endif 
