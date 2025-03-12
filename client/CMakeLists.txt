# Specify the minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(Client)

# Find PkgConfig and ENet
find_package(PkgConfig REQUIRED)
pkg_check_modules(ENet REQUIRED libenet)

# Find SDL2, SDL2_image, and SDL2_ttf
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Include directories from found packages
include_directories(
    ${ENet_INCLUDE_DIRS} 
    ${SDL2_INCLUDE_DIRS} 
    ${SDL2_IMAGE_INCLUDE_DIRS} 
    ${SDL2_TTF_INCLUDE_DIRS}
) 

# Set the source files
set(SOURCES
    src/Button.cpp
    src/CircleCollider.cpp
    src/Entity.cpp 
    src/GameController.cpp
    src/GameView.cpp
    src/InputHandler.cpp
    src/main.cpp
    src/NetworkCommunicator.cpp
    src/Physics.cpp
    src/PhysicsSystem.cpp
    src/Planet.cpp
    src/Player.cpp
    src/RectangleCollider.cpp
    src/TextureManager.cpp
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES}) 

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    PRIVATE ${ENet_LIBRARIES} pthread
    ${SDL2_LIBRARIES} SDL2_ttf SDL2_image
)

# Specify the output directory for the executable
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Ensure correct C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")