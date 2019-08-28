cmake_minimum_required(VERSION 3.10)

project(kyaml)
set(CMAKE_CXX_STANDARD 17)

include(GoogleTest)
find_package(GTest MODULE REQUIRED)
find_library(GMOCK_LIBRARIES gmock)
enable_testing()


# dependencies:

find_package(Composite CONFIG REQUIRED)

find_package(Results CONFIG REQUIRED)


add_subdirectory(lib)
add_subdirectory(test)
add_subdirectory(examples)

# install rules
install(TARGETS kyaml
        EXPORT KyamlConfig
        ARCHIVE DESTINATION lib COMPONENT lib
        PUBLIC_HEADER DESTINATION include/kyaml COMPONENT dev
)

export(TARGETS kyaml NAMESPACE Kyaml:: FILE ${CMAKE_CURRENT_BINARY_DIR}/KyamlConfig.cmake)
install(EXPORT KyamlConfig DESTINATION share/kyaml NAMESPACE Kyaml::)
