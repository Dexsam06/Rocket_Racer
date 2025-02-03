#include "NetworkCommunicator.hpp"
#include <iostream>

int main(int argc, const char * argv[]) {
    std::cout << "Starting server..." << std::endl;

    NetworkCommunicator nc;

    while(true)
    {
        nc.NetworkHandler();
    }

    return 0; 
}