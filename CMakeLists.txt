cmake_minimum_required(VERSION 3.20)

project(RakNet LANGUAGES CXX)

find_package(Threads REQUIRED)

add_library(
    raknet
    STATIC

    source/BitStream.cpp
    source/CheckSum.cpp
    source/DataBlockEncryptor.cpp
    source/DS_HuffmanEncodingTree.cpp
    source/GetTime.cpp
    source/InternalPacketPool.cpp
    source/NetworkTypes.cpp
    source/RakClient.cpp
    source/RakNetStatistics.cpp
    source/RakPeer.cpp
    source/RakSleep.cpp
    source/Rand.cpp
    source/ReliabilityLayer.cpp
    source/rijndael.cpp
    source/RPCMap.cpp
    source/SHA1.cpp
    source/SimpleMutex.cpp
    source/SocketLayer.cpp
    source/TEABlockEncryptor.cpp
)

target_include_directories(
    raknet
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    raknet
    PUBLIC
        Threads::Threads
)

if(WIN32)
    target_compile_definitions(
        raknet
        PRIVATE
            WIN32_LEAN_AND_MEAN
            NOMINMAX
    )

    target_link_libraries(
        raknet
        PUBLIC
            ws2_32
    )
endif()

