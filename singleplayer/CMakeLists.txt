# Specify the minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(Rocket_Racer)

# Find the SDL2, SDL2_image, and SDL2_ttf packages
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)  # Ensure SDL2_ttf is found

# Include directories for SDL2, SDL2_image, and SDL2_ttf
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})

# Set the source files 
set(SOURCES
    src/Button.cpp
    src/CircleCollider.cpp  
    src/Entity.cpp 
    src/GameController.cpp
    src/GameView.cpp
    src/InputHandler.cpp
    src/main.cpp
    src/Physics.cpp
    src/PhysicsSystem.cpp
    src/Planet.cpp
    src/Player.cpp
    src/RectangleCollider.cpp
    src/TextureManager.cpp
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link SDL2, SDL2_image, and SDL2_ttf explicitly
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} SDL2_image SDL2_ttf)

# Specify the output directory for the executable
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
