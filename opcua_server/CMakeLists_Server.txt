


cmake_minimum_required(VERSION 2.8.4)
project(OPCUACppSDK)




#ROOT = ../../../..
set(PROJECT_BASE "${CMAKE_CURRENT_SOURCE_DIR}")
set(SDK_BASE "${PROJECT_BASE}/OpcUACppSDK/Source/")
set(CMAKE_INSTALL_PREFIX ${PROJECT_BASE})
set(INTSTALL_DIR "bin")
set(LIB_DIR "lib")


message(PROJECT_BASE: ${PROJECT_BASE})
message(SDK_BASE: ${SDK_BASE})





## Cmake rules. DO NOT MODIFY unless you know what you are doing !
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_C_COMPILER gcc)	
set(CMAKE_STRIP_COMMAND strip)

set(CMAKE_MACHINE_TYPE x86_64)
set(CMAKE_MACHINE_OPT -m64)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GNU_SOURCE -D_REENTRANT -D_THREAD_SAFE -D_FILE_OFFSET_BITS=32")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GNU_SOURCE -O3 -DLOGGING_ENABLED=1 -O3 -fomit-frame-pointer -m64")
message(CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS})

set(CMAKE_CXX_LDFLAGS "-O3 -DLOGGING_ENABLED=1 -O3 -fomit-frame-pointer -m64")
message(CMAKE_CXX_LDFLAGS: ${CMAKE_CXX_LDFLAGS})
set(CMAKE_C_LDFLAGS " ")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLOGGING_ENABLED=1 -O3 -fomit-frame-pointer -m64")
#set(CMAKE_C_FLAGS " ")

set(LINK_SDK_LIB_DIR  "${SDK_BASE}/lib/linux/x86_64/gcc9/release")


add_subdirectory(Server)
