cmake_minimum_required(VERSION 3.20.0)
project(base64 VERSION 0.1.0)

add_executable(base64 main.cpp)

set_target_properties(
  base64
  PROPERTIES
  CXX_STANDARD 17
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
