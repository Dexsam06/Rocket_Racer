#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <SDL2/SDL.h>
#include <functional>
#include <SDL2/SDL_ttf.h>

class Button {
public:
    // Define a type for the callback function
    using ButtonCallback = std::function<void()>;

    Button(int x, int y, int w, int h, SDL_Color color, const char* text, SDL_Renderer* renderer, TTF_Font* font, ButtonCallback callback = nullptr);
    ~Button();

    void render();
    void handleEvent(SDL_Event& event);

    bool isClicked(int mouseX, int mouseY);
    
    void setCallback(ButtonCallback callback); // Set a new callback

private:
    int x, y, w, h;
    SDL_Color color;
    const char* text;
    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Texture* textTexture;
    ButtonCallback callback; // Store the callback 
};

#endif