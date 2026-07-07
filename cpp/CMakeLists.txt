# Cloth simulation core — C++17, header-only library + tests + headless demo.
#
#   cmake -S . -B build -G Ninja
#   cmake --build build
#   ctest --test-dir build --output-on-failure
#
# Catch2 v2 (single header) is vendored under third_party/ so the build works
# offline; no FetchContent required.

cmake_minimum_required(VERSION 3.16)
project(cloth_sim CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra)
endif()

# Header-only core
add_library(cloth INTERFACE)
target_include_directories(cloth INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Headless demo (also doubles as a CI smoke check — exits 1 if unstable)
add_executable(cloth_headless src/demo_headless.cpp)
target_link_libraries(cloth_headless PRIVATE cloth)

# Unit tests (Catch2 v2, vendored single header)
enable_testing()
add_executable(cloth_tests tests/test_cloth.cpp)
target_include_directories(cloth_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/catch2)
target_link_libraries(cloth_tests PRIVATE cloth)

add_test(NAME unit     COMMAND cloth_tests)
add_test(NAME headless COMMAND cloth_headless)
