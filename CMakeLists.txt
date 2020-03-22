cmake_minimum_required(VERSION 3.15)
set(CMAKE_C_STANDARD 11)

include_directories(shared/include)


project(daemon C)
add_executable(daemon daemon/src/server.c shared/src/myfs.c shared/src/web.c)
target_include_directories(daemon PRIVATE daemon/include)


project(client C)
add_executable(client client/src/prepare_request.c client/src/client.c shared/src/myfs.c
        shared/src/web.c )
target_include_directories(client PRIVATE client/include)
