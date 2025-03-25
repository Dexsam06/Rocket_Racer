#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <SDL2/SDL.h>
#include <memory>
#include <vector>

#include "Player.hpp"
#include "Button.hpp"
#include "NetworkPackets.hpp"

class InputHandler
{

public:
    void handleInput(std::vector<std::unique_ptr<Button>> &buttonList);
    bool isQuit() const { return quit; }

    using InputPacketCallback = std::function<void(ClientInputPacket &)>;

    void setInputPacketCallback(InputPacketCallback callback) {inputPacketCallback = callback; }

    void sendInputToServer(ClientInputPacket packet) {
        if (inputPacketCallback) inputPacketCallback(packet);
    }

private:
    bool quit = false;

    InputPacketCallback inputPacketCallback;
};

#endif
