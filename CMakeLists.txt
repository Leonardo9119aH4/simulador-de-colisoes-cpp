cmake_minimum_required(VERSION 3.16)
project(SimuladorDeColisoes CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    # ALTERADO AQUI: apontando para a pasta de 32-bits
    set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/SDL3-3.4.8/i686-w64-mingw32")
endif()

find_package(SDL3 REQUIRED CONFIG)

file(GLOB SOURCES "src/*.cpp")
add_executable(SimuladorDeColisoes ${SOURCES})
target_link_libraries(SimuladorDeColisoes PRIVATE SDL3::SDL3)

if(WIN32)
    add_custom_command(TARGET SimuladorDeColisoes POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        # ALTERADO AQUI TAMBÉM: copiando a DLL de 32-bits
        "${CMAKE_CURRENT_SOURCE_DIR}/SDL3-3.4.8/i686-w64-mingw32/bin/SDL3.dll"
        $<TARGET_FILE_DIR:SimuladorDeColisoes>
    )
endif()