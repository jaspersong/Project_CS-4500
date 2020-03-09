cmake_minimum_required(VERSION 2.8.2)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Threads REQUIRED)

configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(
    COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download
    )

if (result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()

execute_process(
    COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download
    )

if (result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL
                 )

# Set the compile flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pthread")

include_directories(
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/dataframe"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/messages"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/network"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests")

# Create targets for the test suites
# TODO: Bring in the test suite for the sorer
add_executable(test_suite_array
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_array.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/array.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/custom_string.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/helper.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/object.h")
add_executable(test_suite_map
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_map.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/map.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/custom_string.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/helper.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/object.h")
add_executable(test_suite_queue
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_queue.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/queue.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/custom_string.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/helper.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/object.h")
add_executable(test_suite_dataframe
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_dataframe.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/custom_string.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/helper.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/object.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/thread.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/dataframe/column.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/dataframe/row.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/dataframe/schema.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/dataframe/dataframe.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/dataframe/dataframe_helper.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/dataframe/fielder.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/dataframe/rower.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/dataframe/summation_rower.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/dataframe/euclidean_magnitude_rower.h")
target_link_libraries(test_suite_dataframe gtest)
add_executable(test_suite_messages
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_messages.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/custom_string.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/helper.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/object.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/messages/serial.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/messages/serialized_arrays.h")
target_link_libraries(test_suite_messages gtest)
add_executable(test_suite_network
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_network.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/queue.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/custom_string.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/helper.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/object.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/thread.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/network/network.h")
target_link_libraries(test_suite_network gtest)

# Create targets to demo server/client and registrar/node functionality
add_executable(demo_echo_server
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/echo_server.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/queue.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/custom_string.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/helper.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/object.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/thread.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/network/network.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/network/echo_network.h")
add_executable(demo_echo_client
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/echo_client.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/queue.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/custom_string.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/helper.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/object.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/thread.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/network/network.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/network/echo_network.h")
add_executable(demo_registrar
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/registrar.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/queue.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/custom_string.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/helper.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/object.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/thread.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/network/network.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/network/distributed_app.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/messages/serial.h")
add_executable(demo_node
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/node.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/queue.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/custom_string.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/helper.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/object.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/thread.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/network/network.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/network/distributed_app.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/messages/serial.h")
