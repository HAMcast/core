cmake_minimum_required(VERSION 2.6)
project (hamcast)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHC_ENABLE_LOGGING -g -Wextra -Wall -Wno-c++11-long-long -Wno-unused-parameter -Wno-unused-function -Wno-unused-parameter")
### exchange these lines if compiling on planetlab or other x86 systems
#    set (CMAKE_CXX_FLAGS "-00 -DHC_ENABLE_LOGGING -Wextra -Wall -pedantic -march=i486")
else ()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
### exchange these lines if compiling on planetlab or other x86 systems
#    set (CMAKE_CXX_FLAGS "-O2 -march=i486")
endif (CMAKE_BUILD_TYPE STREQUAL Debug)

set (HAMCAST_BINARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/../bin)
set (HAMCAST_LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/../lib)

set (LIBRARY_OUTPUT_PATH ${HAMCAST_LIBRARY_OUTPUT_PATH}
        CACHE PATH "Single directory for all libraries")
set (EXECUTABLE_OUTPUT_PATH ${HAMCAST_BINARY_OUTPUT_PATH}
        CACHE PATH "Single directory for all executables")

mark_as_advanced (
    LIBRARY_OUTPUT_PATH
    EXECUTABLE_OUTPUT_PATH
)

# parse bufer size flags
if (DEFAULT_MAX_BUFFER_SIZE)
    add_definitions(-DHC_DEFAULT_MAX_BUFFER_SIZE=${DEFAULT_MAX_BUFFER_SIZE})
    message (STATUS "set default max buffer size to ${DEFAULT_MAX_BUFFER_SIZE} bytes")
endif (DEFAULT_MAX_BUFFER_SIZE)

if (MAX_CHANNEL_QUEUE_SIZE)
    add_definitions (-DHC_MAX_CHANNEL_QUEUE_SIZE=${MAX_CHANNEL_QUEUE_SIZE})
    message (STATUS "set max queue size to ${MAX_CHANNEL_QUEUE_SIZE} bytes")
endif (MAX_CHANNEL_QUEUE_SIZE)

# setting path to hamcast headers and libhamcast
set (HAMCAST_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/libhamcast/include)
set (HAMCAST_LIBRARY_PATH ${HAMCAST_LIBRARY_OUTPUT_PATH})
set (HAMCAST_INCLUDE ${HAMCAST_INCLUDE_PATH})
if (APPLE)
    set (HAMCAST_LIBRARY ${HAMCAST_LIBRARY_PATH}/libhamcast.dylib)
elseif (UNIX)
    set (HAMCAST_LIBRARY ${HAMCAST_LIBRARY_PATH}/libhamcast.so)
else ()
    message (SEND_FATAL "Host platform not supported ...")
endif ()

add_subdirectory (modules/utils/chimera)
add_subdirectory (libhamcast)

add_subdirectory (middleware)
add_dependencies (middleware libhamcast)

add_subdirectory (modules/loopback)
add_dependencies (libloopbackmodule libhamcast)
add_subdirectory (modules/scribe)
add_dependencies (libscribemodule modules/utils/chimera libhamcast)
add_subdirectory (modules/tunnel)
add_dependencies (libtunnelmodule libhamcast)
add_subdirectory (modules/void)
add_dependencies (libvoidmodule libhamcast)
