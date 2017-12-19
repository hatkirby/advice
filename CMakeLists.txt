cmake_minimum_required (VERSION 3.1)
project (advice)

set(CMAKE_BUILD_TYPE Debug)

find_package(PkgConfig)
pkg_check_modules(GraphicsMagick GraphicsMagick++ REQUIRED)
pkg_check_modules(yaml-cpp yaml-cpp REQUIRED)

add_subdirectory(vendor/verbly)
add_subdirectory(vendor/libtwittercpp)

include_directories(
  vendor/verbly/lib
  vendor/libtwittercpp/src
  vendor/libtwittercpp/vendor/curlcpp/include
  ${GraphicsMagick_INCLUDE_DIRS}
  ${yaml-cpp_INCLUDE_DIRS})

add_executable(advice main.cpp advice.cpp sentence.cpp)
set_property(TARGET advice PROPERTY CXX_STANDARD 11)
set_property(TARGET advice PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(advice verbly twitter++ ${GraphicsMagick_LIBRARIES} ${yaml-cpp_LIBRARIES})
