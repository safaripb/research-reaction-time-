# Tell CMake the minimum version this ESP-IDF project expects.
cmake_minimum_required(VERSION 3.22)

# Load ESP-IDF's CMake build system.
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Define the project name.
project(reaction_tester)