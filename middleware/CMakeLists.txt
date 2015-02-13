cmake_minimum_required(VERSION 2.6)
project (hamcast_middleware)

# Set up environment paths to cmake modules and libhamcast
set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

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

set (MIDDLEWARE_SRC src/connection.cpp 
                    src/middleware.cpp
                    src/rw_spinlock.cpp
                    src/send_job.cpp
                    src/session.cpp 
                    src/socket_proxy.cpp
                    src/tech_module.cpp 
                    src/main.cpp)

# search for libs
if (NOT HAMCAST_LIBRARY)
    find_package (LibHamcast REQUIRED)
endif (NOT HAMCAST_LIBRARY)

find_package (Threads)

include_directories ( . ./include ${HAMCAST_INCLUDE} )

add_executable (middleware ${MIDDLEWARE_SRC})

target_link_libraries (middleware ${CMAKE_DL_LIBS}
                                  ${CMAKE_THREAD_LIBS_INIT} 
                                  ${HAMCAST_LIBRARY} )
