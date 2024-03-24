# CMake Build Configuration for root of project
cmake_minimum_required(VERSION 3.18)
project(CMPT433_ALLIN VERSION 1.0 DESCRIPTION "CMPT433_ALLIN" LANGUAGES C)

# Compiler options (inherited by sub-folders)
set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -Werror -Wpedantic -Wextra)
add_compile_options(-fdiagnostics-color)

# Enable address sanitizer
# (Comment this out to make your code faster)
#add_compile_options(-fsanitize=address)
#add_link_options(-fsanitize=address)
find_package(Threads REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/app/include)
include_directories(${PROJECT_SOURCE_DIR}/app/src)
include_directories(${PROJECT_SOURCE_DIR}/hal/include)
include_directories(${PROJECT_SOURCE_DIR}/hal/src)


# What folders to build
add_subdirectory(hal)  
add_subdirectory(app)


