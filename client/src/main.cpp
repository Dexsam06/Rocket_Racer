#include "NetworkCommunicator.hpp"
#include <iostream>

int main(int argc, char **argv)
{

    std::cout << "Skriv ip address på servern: " << std::endl;
    std::string address;
    std::cin >> address;

    NetworkCommunicator nc(address);
    nc.NetworkHandler();
    
    return 0; 
}

