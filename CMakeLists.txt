cmake_minimum_required(VERSION 3.15)
set(CMAKE_C_STANDARD 11)

include_directories(shared/include)

project(filesystem C)
add_executable(filesystem
        filesystem/src/driver.c
        filesystem/src/filesystem.c
        filesystem/src/operations.c
        filesystem/src/utils.c
        shared/src/myfs.c
        shared/src/data_structures.c)
target_include_directories(filesystem PRIVATE filesystem/include)

project(daemon C)
add_executable(daemon
        daemon/src/server.c
        shared/src/myfs.c)
target_include_directories(daemon PRIVATE daemon/include)

project(client C)
add_executable(client
        client/src/prepare_request.c
        client/src/client.c
        shared/src/myfs.c)
target_include_directories(client PRIVATE client/include)
