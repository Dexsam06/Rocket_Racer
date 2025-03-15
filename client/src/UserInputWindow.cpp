#include "../include/UserInputWindow.hpp"

// Helper function to trim leading and trailing spaces
std::string trim(const std::string &str) {
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    return (first == std::string::npos || last == std::string::npos) ? "" : str.substr(first, (last - first + 1));
}

// IP address validation with regex
bool validateIPAddress(const std::string &ip) {
    std::string trimmedIP = trim(ip);  // Trim spaces from the input string
    
    std::regex ipPattern(R"(^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$)");


    bool isValid = std::regex_match(trimmedIP, ipPattern);
    std::cout << "Trimmed IP: [" << trimmedIP << "] Valid: " << isValid << std::endl; // Debug output
    return isValid;
}

// Username validation (non-empty and max 30 chars)
bool validateUsername(const std::string &username) {
    return !username.empty() && username.length() <= 30;
}

// Input window loop to gather user input for IP and username
bool getUserInput(std::string &serverIP, std::string &username) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() < 0) {
        std::cerr << "SDL or TTF initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }

    const int WIDTH = 500, HEIGHT = 300;
    SDL_Window *window = SDL_CreateWindow("Enter Server Details", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font *font = TTF_OpenFont("../../res/Fonts/Roboto-Regular.ttf", 24);

    if (!window || !renderer || !font) {
        std::cerr << "SDL Window, Renderer, or Font creation failed!" << std::endl;
        return false;
    }

    SDL_StartTextInput();
    std::string currentText = "";
    bool enteringIP = true;
    bool done = false;

    while (!done) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return false;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
                std::cout << "Entered text: [" << currentText << "]" << std::endl; // Debug output
                
                // Trim spaces (remove leading & trailing spaces)
                currentText = trim(currentText);
                
                if (enteringIP) {
                    if (validateIPAddress(currentText)) {
                        std::cout << "Valid IP entered!" << std::endl;
                        serverIP = currentText;
                        currentText = "";
                        enteringIP = false; // Move to username input
                    } else {
                        std::cout << "Invalid IP address!" << std::endl;
                    }
                } else {
                    if (validateUsername(currentText)) {
                        std::cout << "Valid username entered!" << std::endl;
                        username = currentText;
                        done = true;
                    } else {
                        std::cout << "Invalid username!" << std::endl;
                    }
                }
            }

            if (e.type == SDL_TEXTINPUT) {
                currentText += e.text.text;
            }
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_BACKSPACE && !currentText.empty()) {
                currentText.pop_back();
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        std::string prompt = enteringIP ? "Enter Server IP: " : "Enter Username:";
        SDL_Color color = {255, 255, 255};

        SDL_Surface *textSurface = TTF_RenderText_Solid(font, (prompt + currentText).c_str(), color);
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {50, HEIGHT / 3, textSurface->w, textSurface->h};

        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_RenderPresent(renderer);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }

    SDL_StopTextInput();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    SDL_Quit();
    TTF_Quit();

    return true;
}