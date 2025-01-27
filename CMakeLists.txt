cmake_minimum_required(VERSION 3.29 FATAL_ERROR)
project(CServer VERSION 0.1.0 LANGUAGES CXX)

set(OSBitness 32)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(OSBitness 64)
endif()

set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_C_STANDARD 90)

if(UNIX)
    set(CMAKE_C_FLAGS "-Wall -Wextra")
    set(CMAKE_C_FLAGS_DEBUG "-g")
    set(CMAKE_C_FLAGS_RELEASE "-O3")
elseif(WIN32)
    set(CMAKE_C_FLAGS_RELEASE "-O2")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS true)
    set(BUILD_SHARED_LIBS true)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

include(FetchContent)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

set(FullOutputDir "${CMAKE_SOURCE_DIR}/.bin/${CMAKE_SYSTEM_NAME}${OSBitness}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${FullOutputDir}/static_lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${FullOutputDir}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${FullOutputDir}/bin")

add_subdirectory(app)
add_subdirectory(lib)
