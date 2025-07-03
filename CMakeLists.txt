cmake_minimum_required(VERSION 3.31)
project(binance_book_club)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)
find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)

include(FetchContent)
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.13.0
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
        cpr
        GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 1.9.0  # Or latest stable tag
)
FetchContent_MakeAvailable(cpr)

FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(json)

file(GLOB_RECURSE SRC_FILES "src/*.cpp")
message(STATUS "Source files: ${SRC_FILES}")

add_executable(binance_book_club
                    ${SRC_FILES}
                    main.cpp)

target_link_libraries(binance_book_club PRIVATE
        Boost::system
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        OpenSSL::SSL
        OpenSSL::Crypto
        Threads::Threads
        CURL::libcurl
        cpr::cpr
)