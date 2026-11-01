cmake_minimum_required(VERSION 3.20)
project(MiniGit VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Output directories ────────────────────────────────────────────────────────
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ── Dependencies ──────────────────────────────────────────────────────────────
include(FetchContent)

# nlohmann/json
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# OpenSSL (for SHA-256)
find_package(OpenSSL REQUIRED)

# ── Core library ──────────────────────────────────────────────────────────────
file(GLOB_RECURSE CORE_SOURCES
    src/utils/*.cpp
    src/core/*.cpp
    src/services/*.cpp
    src/cli/*.cpp
    src/commands/*.cpp
)

add_library(minigit_core STATIC ${CORE_SOURCES})
target_include_directories(minigit_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(minigit_core PUBLIC
    nlohmann_json::nlohmann_json
    OpenSSL::Crypto
)
if(WIN32)
    target_link_libraries(minigit_core PUBLIC ws2_32)
endif()

# ── Main executable ───────────────────────────────────────────────────────────
add_executable(minigit src/main.cpp)
target_include_directories(minigit PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(minigit PRIVATE minigit_core)

# ── Install ───────────────────────────────────────────────────────────────────
install(TARGETS minigit DESTINATION bin)
