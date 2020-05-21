#!/bin/bash

CLIENT_PATH="../cmake-build-debug/client"

$CLIENT_PATH ls /
$CLIENT_PATH cd /
$CLIENT_PATH cp-upload ~/myfs/CMakeLists.txt /CMakeLists.txt
$CLIENT_PATH mkdir /temp/
$CLIENT_PATH cp /CMakeLists.txt /temp/CMakeLists.txt
$CLIENT_PATH cat /temp/CMakeLists.txt
$CLIENT_PATH cp-download /temp/CMakeLists.txt ~/myfs/CMakeLists_.txt
$CLIENT_PATH rm /temp/CMakeLists.txt
$CLIENT_PATH mv /CMakeLists.txt /temp/CMakeLists.txt
$CLIENT_PATH rm /temp/CMakeLists.txt
