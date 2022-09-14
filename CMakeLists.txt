cmake_minimum_required(VERSION 3.14)
include(cmake/prefix.cmake)
project(lattice CXX)
include(cmake/postfix.cmake)

add_library(lattice INTERFACE)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  include(googletest)
endif()
FetchContent_MakeAvailable(${FetchContents})

find_package(Boost)

# if(NOT (TARGET eigen))
#   message(SEND_ERROR "Attempt to link to non-existent library eigen")
# endif()

if(NOT (TARGET Eigen3::Eigen))
  message(SEND_ERROR "Attempt to link to non-existent library Eigen3::Eigen")
endif()
target_link_libraries(lattice INTERFACE Eigen3::Eigen)
target_include_directories(lattice INTERFACE ${PROJECT_SOURCE_DIR})


if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  add_subdirectory(test)
  add_subdirectory(example)
endif()
