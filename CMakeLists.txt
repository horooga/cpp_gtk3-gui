cmake_minimum_required(VERSION 3.16)

project(cpp_gtk3-gui LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

add_executable(cpp_gtk3-gui src/main.cpp src/utils.cpp)

target_include_directories(cpp_gtk3-gui PRIVATE ${GTKMM_INCLUDE_DIRS} include)
target_link_directories(cpp_gtk3-gui PRIVATE ${GTKMM_LIBRARY_DIRS})
target_compile_options(cpp_gtk3-gui PRIVATE ${GTKMM_CFLAGS_OTHER})
target_link_libraries(cpp_gtk3-gui PRIVATE ${GTKMM_LIBRARIES})
