cmake_minimum_required(VERSION 3.20)

project(Raknet LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

add_library(
    Raknet
    STATIC

    source/BitStream.cpp
    source/CheckSum.cpp
    source/DS_HuffmanEncodingTree.cpp
    source/InternalPacketPool.cpp
    source/NetworkTypes.cpp
    source/RakClient.cpp
    source/RakNetStatistics.cpp
    source/RakPeer.cpp
    source/RakServer.cpp
    source/ReliabilityLayer.cpp
    source/RPCMap.cpp
    source/SHA1.cpp
    source/SocketLayer.cpp
    source/StringCompressor.cpp
    source/StringTable.cpp
    source/TEABlockEncryptor.cpp
)

target_include_directories(
    Raknet
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    Raknet
    PUBLIC
        Threads::Threads
)

if(WIN32)
    target_compile_definitions(
        Raknet
        PRIVATE
            WIN32_LEAN_AND_MEAN
            NOMINMAX
    )

    target_link_libraries(
        Raknet
        PUBLIC
            ws2_32
    )
endif()

