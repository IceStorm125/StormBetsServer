cmake_minimum_required(VERSION 3.22)

project(StormBetsServer LANGUAGES CXX)

if (CONAN_RESULT)
    message(FATAL_ERROR "Failed to run conan install command")
endif ()

include("${CMAKE_CURRENT_BINARY_DIR}/Release/generators/conan_toolchain.cmake")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(tgbot-cpp)

find_package(Qt5Core REQUIRED)
find_package(Qt5Sql REQUIRED)

find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(fmt REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(GTest REQUIRED)
find_package(date REQUIRED)


include_directories(
        include
        include/*
)

set(include_files
        include/match.h
        include/keyboardcreator.h
        include/processing.h
        include/messagestosend.h
        include/dtos/playerdto.h
        include/regexmatcher.h
        include/dtos/dto.h
        include/dtos/betdto.h
        include/dtos/admindto.h
        include/dtos/matchdto.h
        include/dbconnection.h
        include/messageeventsgenerator.h
)

set(source_files
        src/match.cpp
        src/keyboardcreator.cpp
        src/processing.cpp
        src/regexmatcher.cpp
        src/dtos/playerdto.cpp
        src/dtos/dto.cpp
        src/dtos/betdto.cpp
        src/dtos/admindto.cpp
        src/dtos/matchdto.cpp
        src/dbconnection.cpp
        src/EnvManager.hpp
        src/messageeventsgenerator.cpp
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
        nlohmann_json::nlohmann_json
        date::date
)

enable_testing()
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
        nlohmann_json::nlohmann_json
        gtest::gtest
        date::date
)

