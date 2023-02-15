cmake_minimum_required(VERSION 3.5)

project(StormBetsServer LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(CURL REQUIRED)

find_package(Qt5Core)
find_package(Qt5Test)
find_package(Qt5Sql)

add_subdirectory(tgbot-cpp)

add_executable(StormBetsServer
    main.cpp
    requester.h requester.cpp
    match.h match.cpp
    jsondataextractor.h jsondataextractor.cpp
    keyboardcreator.h keyboardcreator.cpp
    event.h
    processing.h processing.cpp
    messagestosend.h
    messageeventsgenerator.hpp
    regexmatcher.h regexmatcher.cpp
    playerdto.h playerdto.cpp
    tests/test_regexmatcher.h tests/test_regexmatcher.cpp
    dto.h dto.cpp
    betdto.h betdto.cpp
    admindto.h admindto.cpp
    )

target_link_libraries(StormBetsServer
    Qt5::Core
    Qt5::Test
    Qt5::Sql
    TgBot
    ${CMAKE_THREAD_LIBS_INIT}
    ${OPENSSL_LIBRARIES}
    ${Boost_LIBRARIES}
    ${CURL_LIBRARIES}
    )
