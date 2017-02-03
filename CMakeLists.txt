cmake_minimum_required (VERSION 3.1)
project (advice)

set(CMAKE_BUILD_TYPE Debug)

find_package(PkgConfig)
pkg_check_modules(GraphicsMagick GraphicsMagick++ REQUIRED)

add_subdirectory(vendor/verbly)
add_subdirectory(vendor/libtwittercpp)
add_subdirectory(vendor/yaml-cpp EXCLUDE_FROM_ALL)

include_directories(vendor/verbly/lib vendor/libtwittercpp/src vendor/libtwittercpp/vendor/curlcpp/include ${GraphicsMagick_INCLUDE_DIRS} vendor/yaml-cpp/include)
add_executable(advice main.cpp advice.cpp sentence.cpp)
set_property(TARGET advice PROPERTY CXX_STANDARD 11)
set_property(TARGET advice PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(advice verbly twitter++ ${GraphicsMagick_LIBRARIES} yaml-cpp)
