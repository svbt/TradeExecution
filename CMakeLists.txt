cmake_minimum_required(VERSION 3.15)
project(TradingMicroservice CXX)

set(CMAKE_CXX_STANDARD 17)

# Configure Conan integration
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)

# Find packages provided by Conan
find_package(Pistache REQUIRED)
find_package(nlohmann_json REQUIRED)

# Add source files
add_executable(TradingMicroservice
    src/main.cpp
    src/ExecutionEngine.cpp
    src/TradingMicroservice.cpp
)

# Link libraries
target_link_libraries(TradingMicroservice
    PRIVATE
    Pistache::Pistache
    nlohmann_json::nlohmann_json
)

