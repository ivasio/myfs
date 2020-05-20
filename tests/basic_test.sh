#!/bin/bash

CLIENT_PATH="../cmake-build-debug/client"

$CLIENT_PATH ls /
#sleep 3
$CLIENT_PATH cd /
#sleep 3
$CLIENT_PATH cp-upload ~/myfs/CMakeLists.txt /CMakeLists.txt
#sleep 3
$CLIENT_PATH mkdir /temp/
#sleep 3
$CLIENT_PATH cp /CMakeLists.txt /temp/CMakeLists.txt
#sleep 3
$CLIENT_PATH cat /temp/CMakeLists.txt
#sleep 3
$CLIENT_PATH cp-download /temp/CMakeLists.txt ~/myfs/CMakeLists_.txt
#sleep 3
$CLIENT_PATH rm /temp/CMakeLists.txt
#sleep 3
$CLIENT_PATH mv /CMakeLists.txt /temp/CMakeLists.txt
#sleep 3
$CLIENT_PATH rm /temp/CMakeLists.txt
#sleep 3
