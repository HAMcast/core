cmake_minimum_required(VERSION 2.6)
project (hamcast_middleware)

# Set up environment paths to cmake modules and libhamcast
set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -std=c++11 -DHC_ENABLE_LOGGING -g -Wextra -Wall -pedantic")
### exchange these lines if compiling on planetlab or other x86 systems
#    set (CMAKE_CXX_FLAGS "-DHC_ENABLE_LOGGING -Wextra -Wall -pedantic -march=i486")
else ()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -std=c++11 -O3")
### exchange these lines if compiling on planetlab or other x86 systems
#    set (CMAKE_CXX_FLAGS "-O2 -march=i486")
endif (CMAKE_BUILD_TYPE STREQUAL Debug)

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
