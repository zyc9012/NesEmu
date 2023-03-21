cmake_minimum_required(VERSION 3.0)

project(NesEmu)

if (NOT MSVC)
  set(CMAKE_CXX_FLAGS "-O3 -Wno-unused-result --std=c++11")
  if (NOT APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "-static -s")
  endif()
endif()

if (MSVC AND WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE")
endif()

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src)

file(GLOB_RECURSE SRCS RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp")

add_executable(NesEmu ${SRCS})

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(NesEmu SDL2::SDL2-static)
