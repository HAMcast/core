cmake_minimum_required(VERSION 2.6)
project (hamcast_libvoid_module)

# Set up environment paths to cmake modules and libhamcast
set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

#set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DHC_ENABLE_LOGGING -g -Wextra -Wall -pedantic")
else ()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")
endif (CMAKE_BUILD_TYPE STREQUAL Debug)
### uncomment if compiling for planetlab or other x86 systems
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=i486")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

# search for libs
find_package (LibHamcast REQUIRED)

include_directories (. ${HAMCAST_INCLUDE} )

add_library(libloopbackmodule SHARED loopback.cpp)

target_link_libraries (libloopbackmodule ${CMAKE_LD_LIBS} ${HAMCAST_LIBRARY})

# install lib
set (LIBLOOP_MODULE_VERSION_MAJOR 0)
set (LIBLOOP_MODULE_VERSION_MINOR 15)
set (LIBLOOP_MODULE_VERSION_PATCH 1)
set(LIBRARY_VERSION ${LIBLOOP_MODULE_VERSION_MAJOR}.${LIBLOOP_MODULE_VERSION_MINOR}.${LIBLOOP_MODULE_VERSION_PATCH})
set(LIBRARY_SOVERSION ${LIBLOOP_MODULE_VERSION_MAJOR})
set_target_properties(libloopbackmodule PROPERTIES
                        SOVERSION ${LIBRARY_SOVERSION}
                        VERSION ${LIBRARY_VERSION}
                        OUTPUT_NAME loopbackmodule)
