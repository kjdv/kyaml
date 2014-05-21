# just to surpress a cmake warning, probably lower versions are fine too
cmake_minimum_required(VERSION 2.6)

project(kyaml) # Klaas' yet another markup language implementation

# set the proper compiler flags
include(compilerstuff.cmake)

# find source files
file(GLOB_RECURSE SOURCES src/*.cc src/*.hh)

# put the src subdirectory on the include path
include_directories(${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/src/include)

# put the common code in one library
add_library(${PROJECT_NAME} ${SOURCES})

# sample executable
add_executable(${PROJECT_NAME}_example example/main.cc)
add_dependencies(${PROJECT_NAME}_example ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}_example ${PROJECT_NAME})

# (optional) compile test cases (needs the google test/mock framework)
find_package(GTest)
if(GTEST_FOUND)
  find_path(GMOCK_INCLUDE_DIR gmock/gmock.h)
  find_library(GMOCK_LIBRARIES gmock)

  include_directories(${GTEST_INCLUDE_DIRS}, ${GMOCK_INCLUDE_DIR})

  file(GLOB_RECURSE TEST_SOURCES test/*.cc test/*.hh)

  add_executable(${PROJECT_NAME}_test ${TEST_SOURCES})
  add_dependencies(${PROJECT_NAME}_test ${PROJECT_NAME})

  find_package(Threads) # gtest may also need pthread
  target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME} ${GTEST_BOTH_LIBRARIES} ${GMOCK_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
endif()
