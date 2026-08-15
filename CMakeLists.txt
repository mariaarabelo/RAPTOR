cmake_minimum_required(VERSION 3.22)
project(raptor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Set the default build type to Release if not already set
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the build type" FORCE)
endif()


include_directories(.)

find_package(Threads REQUIRED)

add_subdirectory(tests)

add_library(raptor_lib
        src/Parser.cpp
        src/Raptor.cpp
        src/ParallelRaptor.cpp
        src/Utils.cpp
        src/Application.cpp
        src/DateTime.h
        src/NetworkObjects/DataStructures.h
        src/NetworkObjects/GTFSObjects/Route.cpp
        src/NetworkObjects/GTFSObjects/Trip.cpp
        src/NetworkObjects/GTFSObjects/Stop.cpp
        src/NetworkObjects/GTFSObjects/GTFSObject.cpp
        src/NetworkObjects/GTFSObjects/Agency.cpp
        src/NetworkObjects/GTFSObjects/Calendar.cpp
        src/NetworkObjects/GTFSObjects/StopTime.cpp
)

target_include_directories(raptor_lib PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(raptor_lib PUBLIC Threads::Threads)

add_executable(raptor src/main.cpp)
target_link_libraries(raptor raptor_lib)
