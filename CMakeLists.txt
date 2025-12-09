cmake_minimum_required(VERSION 3.15)
project(TradingMicroservice CXX)

set(CMAKE_CXX_STANDARD 20)  # Required for coroutines
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configure Conan integration
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)

# Find packages provided by Conan (nlohmann_json has config; others use imported targets)
find_package(nlohmann_json REQUIRED)
# No find_package(cppkafka REQUIRED) -- use CONAN_PKG::cppkafka directly via toolchain

# Find CoroKafka (assumes installed via its CMake; adjust path if needed)
find_package(CoroKafka REQUIRED)
find_package(Quantum REQUIRED)  # Bloomberg Quantum for coroutines

# Add source files
add_executable(TradingMicroservice
    src/main.cpp
    src/ExecutionEngine.cpp
    src/TradingMicroservice.cpp
    src/KafkaConsumer.cpp  # New
)

# Link libraries (use Conan imported targets for cppkafka)
target_link_libraries(TradingMicroservice PRIVATE
    Pistache::Pistache
    nlohmann_json::nlohmann_json
    CONAN_PKG::cppkafka  # Conan-generated target
    CoroKafka::corokafka
    Quantum::quantum  # Coroutine dispatcher
)