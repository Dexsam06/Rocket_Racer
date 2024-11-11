# Specify the minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(Rocket_Racer)

# Find the SDL2 and SDL2_image packages
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)  # Ensure SDL2_image is found

# Include directories for SDL2 and SDL2_image
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

# Set the source files
set(SOURCES
    src/Entity.cpp 
    src/GameController.cpp
    src/GameView.cpp
    src/InputHandler.cpp
    src/main.cpp
    src/Physics.cpp
    src/PhysicsSystem.cpp
    src/Planet.cpp
    src/Player.cpp
    src/TextureManager.cpp
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link SDL2 and SDL2_image explicitly
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} SDL2_image)  # Directly linking SDL2_image

# Specify the output directory for the executable
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)