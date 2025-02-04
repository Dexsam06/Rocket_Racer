# Specify the minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(Client)

find_package(PkgConfig REQUIRED)
pkg_check_modules(ENet REQUIRED libenet)

# Include directories from pkg-config
include_directories(${ENet_INCLUDE_DIRS})

# Set the source files
set(SOURCES
    src/main.cpp
    src/NetworkCommunicator.cpp
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
