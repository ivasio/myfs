cmake_minimum_required(VERSION 3.15)
set(CMAKE_C_STANDARD 11)

include_directories(shared/include)

# daemon
project(daemon C)
# file(GLOB shared_sources RELATIVE shared/src *.c)
add_executable(daemon shared/src/myfs.c shared/src/web.c daemon/src/server.c)
target_include_directories(daemon PRIVATE daemon/include)

# client
project(client C)
# file(GLOB shared_sources RELATIVE shared/src *.c)
add_executable(client shared/src/myfs.c shared/src/web.c client/src/client.c  ${shared_sources})
target_include_directories(client PRIVATE client/include)
