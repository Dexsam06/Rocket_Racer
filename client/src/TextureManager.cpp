#include "../include/TextureManager.hpp"

SDL_Texture* TextureManager::loadTexture(const std::string& id, const std::string& filename, SDL_Renderer* renderer) {
    SDL_Surface* tempSurface = IMG_Load(filename.c_str());
    if (!tempSurface) {
        SDL_Log("Failed to load surface: %s", IMG_GetError());
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    return texture;  
}
