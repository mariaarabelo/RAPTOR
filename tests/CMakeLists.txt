# 'tests' is the subproject name
project(tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

enable_testing()

# Include the Google Test submodule
add_subdirectory(lib)
include_directories(${CMAKE_SOURCE_DIR}/tests/lib/googletest/include)

# Add an executable target for tests
add_executable(TESTS
        findJourneys.cpp # Add test files
        parallelRaptor.cpp
)

# Link Google Test libraries and project files
target_link_libraries(TESTS gtest gtest_main raptor_lib)

target_compile_definitions(TESTS PRIVATE DATASET_PATH="${CMAKE_SOURCE_DIR}/datasets")

add_test(NAME GoogleTests COMMAND TESTS)
