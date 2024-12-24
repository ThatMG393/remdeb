cmake_minimum_required(VERSION 3.10)

project(
    RemDeb
    VERSION 0.0.1
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_AS_LIBRARY "Outputs a shared library instead of an executable" ON)

file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/*.c*
    EXCLUDE loader.cpp
)

file(GLOB_RECURSE HEADERS
    ${CMAKE_SOURCE_DIR}/*.h*
)


if(BUILD_AS_LIBRARY)
    find_package(JNI REQUIRED)
    if (JNI_FOUND)
        message (STATUS "JNI_INCLUDE_DIRS=${JNI_INCLUDE_DIRS}")
        message (STATUS "JNI_LIBRARIES=${JNI_LIBRARIES}")
    endif()


    add_library(remdeb SHARED
        ${SOURCES}
        ${HEADERS}
    )
else()
    add_executable(remdeb
        ${SOURCES}
        ${HEADERS}
    )
endif()

include_directories(remdeb
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/jni
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/net
    ${JNI_INCLUDE_DIRS}
)

target_include_directories(remdeb PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/jni
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/net
    ${JNI_INCLUDE_DIRS}
)

target_compile_options(remdeb PRIVATE
    -fPIC
    -Wall
    -Wextra
    -O2
)

target_link_libraries(remdeb
    ${JNI_LIBRARIES}
    pthread
    log
)

message(STATUS "Build Configuration:")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Architecture: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "  Library Build: ${BUILD_AS_LIBRARY}")