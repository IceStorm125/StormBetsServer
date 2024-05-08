cmake_minimum_required(VERSION 3.22)

project(StormBetsServer LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(tgbot-cpp)

find_package(Qt5Core REQUIRED)
find_package(Qt5Sql REQUIRED)

# Check if the command was successful
if(CONAN_RESULT)
    message(FATAL_ERROR "Failed to run conan install command")
endif()

include("${CMAKE_CURRENT_BINARY_DIR}/Release/generators/conan_toolchain.cmake")

find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(fmt REQUIRED)
find_package(CURL REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(GTest REQUIRED)


include_directories(
    include
    include/*
    )

set(include_files
    include/client.h
    include/match.h
    include/jsondataextractor.h
    include/keyboardcreator.h
    include/event.h
    include/processing.h
    include/messagestosend.h
    include/messageeventsgenerator.hpp
    include/dtos/playerdto.h
    include/regexmatcher.h
    include/dtos/dto.h
    include/dtos/betdto.h
    include/dtos/admindto.h
    include/dbconnection.h
    include/curlClient.h
    include/boostClient.h
   )

set(source_files
    src/match.cpp
    src/jsondataextractor.cpp
    src/keyboardcreator.cpp
    src/processing.cpp
    src/regexmatcher.cpp
    src/dtos/playerdto.cpp
    src/dtos/dto.cpp
    src/dtos/betdto.cpp
    src/dtos/admindto.cpp
    src/dbconnection.cpp
    src/curlClient.cpp
    src/boostClient.cpp
)

set(tests_sources
    tests/main_test.cpp
    tests/regex_test.hpp
)

add_executable(StormBetsServer
    src/main.cpp
    ${source_files}
    ${include_files}
)

target_link_libraries(StormBetsServer
    Qt5::Core
    Qt5::Sql
    TgBot
    Boost::system
    spdlog::spdlog 
    fmt::fmt
    CURL::libcurl 
    nlohmann_json::nlohmann_json
)

add_executable(tests
    ${tests_sources}
    ${source_files}
    ${include_files}
)

add_test(tests tests)

target_link_libraries(tests
    Qt5::Core
    Qt5::Sql
    TgBot
    Boost::system
    spdlog::spdlog 
    fmt::fmt
    CURL::libcurl 
    nlohmann_json::nlohmann_json
    gtest::gtest
)

