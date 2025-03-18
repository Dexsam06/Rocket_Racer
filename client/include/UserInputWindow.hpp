#ifndef USERINPUTWINDOW_HPP
#define USERINPUTWINDOW_HPP

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <regex>

bool getUserInput(std::string &serverIP, std::string &username, std::string &resourcePath);

bool validateIPAddress(const std::string &ip);

bool validateUsername(const std::string &username);

#endif // USERINPUTWINDOW_HPP
