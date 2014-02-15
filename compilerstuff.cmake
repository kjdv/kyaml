# Compiler check, enable c++11 features for your compiler. Also enable all warnings.
if(CMAKE_COMPILER_IS_GNUCXX) # GNU
  execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE COMPILER_VERSION)

  if (COMPILER_VERSION GREATER 4.6)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
  elseif(COMPILER_VERSION GREATER 4.2)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall")
  else()
    message(FATAL_ERROR "Using g++ version " ${CMAKE_CXX_COMPILER_VERSION} ", the theoretical minimum is 4.3, tested with 4.6")
  endif()
elseif(CMAKE_CXX_COMPILER MATCHES ".*clang") # CLANG
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
else()
  message(WARNING "The code is tested with gcc and clang, looks like you're using a different compiler. The code should be pretty standards-compliant but your compiler may choke on some of the fancy C++11 stuff.")
endif()
