cmake_minimum_required(VERSION 3.28)

project(StormBetsServer LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5Core)
find_package(Qt5Test)
find_package(Qt5Sql)

add_subdirectory(tgbot-cpp)

include("${CMAKE_CURRENT_BINARY_DIR}/Release/generators/conan_toolchain.cmake")
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(GTest REQUIRED)
# find_package(tgbot REQUIRED)


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
    src/main.cpp
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

set(tests
    tests/test_regexmatcher.h tests/test_regexmatcher.cpp
)

add_executable(StormBetsServer
    ${source_files}
    ${include_files}
    ${tests}
)

target_link_libraries(StormBetsServer
    Qt5::Core
    Qt5::Test
    Qt5::Sql
    TgBot
    Boost::system
    spdlog::spdlog 
    fmt::fmt
    CURL::libcurl 
    openssl::openssl
    gtest::gtest
)

