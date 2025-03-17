# Specify the minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(Server)

find_package(PkgConfig REQUIRED)
pkg_check_modules(ENet REQUIRED libenet)

# Include directories from pkg-config
include_directories(${ENet_INCLUDE_DIRS})

# Set the source files
set(SOURCES
    src/Collider.cpp
    src/Entity.cpp
    src/GameController.cpp
    src/main.cpp
    src/NetworkCommunicator.cpp
    src/Physics.cpp
    src/PhysicsSystem.cpp
    src/Planet.cpp
    src/Player.cpp
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link ENet and pthread libraries from pkg-config
target_link_libraries(${PROJECT_NAME} PRIVATE ${ENet_LIBRARIES} pthread)

# Specify the output directory for the executable
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Ensure correct C++ standard
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")