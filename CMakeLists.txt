cmake_minimum_required(VERSION 3.19)
project(hypredging VERSION 0.7.0 LANGUAGES CXX)

find_package(PkgConfig REQUIRED)
pkg_check_modules(HYPRLAND REQUIRED IMPORTED_TARGET hyprland)

add_library(hypredging SHARED src/main.cpp)
target_compile_features(hypredging PRIVATE cxx_std_23)
target_compile_options(hypredging PRIVATE -Wall -Wextra -Wpedantic -fno-gnu-unique)
target_link_libraries(hypredging PRIVATE PkgConfig::HYPRLAND)
set_target_properties(hypredging PROPERTIES PREFIX "")
