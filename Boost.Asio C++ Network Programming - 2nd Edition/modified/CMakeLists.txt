cmake_minimum_required(VERSION 3.8)
project(asio_net_prog_2nd)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Compiler    version(MS)  version(cmake)
# ----------  -----------  --------------
# VS 2015     14           1900
# VS 2017     15           1910

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_definitions(-DASIO_STANDALONE)

include_directories(third_party/asio/include)

message(STATUS "Setting Compiler options")
if(MSVC)
    add_definitions(-D_WIN32_WINNT=0x0601)
    add_definitions(-D_UNICODE -DUNICODE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
    if(MSVC_VERSION GREATER_EQUAL 1910)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
    endif()
elseif(GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-long-long -pedantic -Werror")
endif()

add_subdirectory(ch4)
add_subdirectory(ch5)
add_subdirectory(ch6)