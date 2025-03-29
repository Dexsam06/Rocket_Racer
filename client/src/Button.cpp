#include "../include/Button.hpp"


Button::Button(int x, int y, int w, int h, SDL_Color color, const char* text, SDL_Renderer* renderer, TTF_Font* font, ButtonCallback callback)
    : x(x), y(y), w(w), h(h), color(color), text(text), renderer(renderer), font(font), callback(callback) 
{
   
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text, {255, 255, 255, 255}); 
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface); 
    SDL_FreeSurface(textSurface); 
}

Button::~Button() {
    SDL_DestroyTexture(textTexture); 
}

void Button::render() {
    SDL_Rect buttonRect = { x, y, w, h };
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a); 
    SDL_RenderFillRect(renderer, &buttonRect); 

    int textWidth, textHeight;
    SDL_QueryTexture(textTexture, nullptr, nullptr, &textWidth, &textHeight);
    SDL_Rect textRect = { x + (w - textWidth) / 2, y + (h - textHeight) / 2, textWidth, textHeight }; 
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect); 
}

void Button::handleEvent(SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        
        if (isClicked(mouseX, mouseY) && callback) { 
            callback();  
        }
    }
}

bool Button::isClicked(int mouseX, int mouseY) {
    return mouseX >= x && mouseX <= (x + w) && mouseY >= y && mouseY <= (y + h);
}

void Button::setCallback(ButtonCallback newCallback) {
    callback = newCallback;
}
