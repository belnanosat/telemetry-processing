cmake_minimum_required(VERSION 2.6)

project(logtocsv)

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_BUILD_TYPE Release)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

set(${PROJECT_NAME}_VERSION_MAJOR 0)
set(${PROJECT_NAME}_VERSION_MINOR 0)
set(${PROJECT_NAME}_VERSION_PATCH 1)
set(${PROJECT_NAME}_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}")
message(STATUS "${PROJECT_NAME} ${${PROJECT_NAME}_VERSION}")

set(CMAKE_DEBUG_POSTFIX "_d")

set(CMAKE_C_FLAGS "-Wall")
set(CMAKE_C_FLAGS_DEBUG "-g")

set(${PROJECT_NAME}_INCLUDE_DIRS
  include/)

include_directories(
	${${PROJECT_NAME}_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/nanopb/
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
)

# Update nanopb files
set(NANOPB_SRCS
    nanopb/pb_encode.c
    nanopb/pb_decode.c
    nanopb/pb_common.c)

# Update proto file
execute_process(COMMAND protoc --plugin=protoc-gen-nanopb=${CMAKE_CURRENT_SOURCE_DIR}/nanopb/generator/protoc-gen-nanopb
                       --nanopb_out=${CMAKE_CURRENT_SOURCE_DIR}/src/proto/
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/proto/telemetry.proto
                       --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/src/proto/)

add_subdirectory(src)

add_executable(${PROJECT_NAME} ${SRCS} ${NANOPB_SRCS} src/proto/telemetry.pb.c)
