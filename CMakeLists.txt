cmake_minimum_required(VERSION 2.8.2)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the compile flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pthread")
#add_definitions(-DDEBUG_NETWORK_PRINT)

include_directories(
        "${CMAKE_CURRENT_SOURCE_DIR}/inc"
        "${CMAKE_CURRENT_SOURCE_DIR}/inc/application"
        "${CMAKE_CURRENT_SOURCE_DIR}/inc/dataframe"
        "${CMAKE_CURRENT_SOURCE_DIR}/inc/messages"
        "${CMAKE_CURRENT_SOURCE_DIR}/inc/network"
        "${CMAKE_CURRENT_SOURCE_DIR}/inc/utility"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/application"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/dataframe"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/messages"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/network"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/dataframe/CS4500-A1-part1")

set(UTILITY_SRC
        inc/utility/helper.h
        inc/utility/thread.h
        inc/utility/custom_object.h
        inc/messages/serializer.h
        src/messages/serializer.cpp
        inc/utility/custom_string.h
        src/utility/custom_string.cpp
        )
set(DATAFRAME_SRC
        inc/dataframe/dataframe_column.h
        src/dataframe/dataframe_column.cpp
        inc/dataframe/row.h
        src/dataframe/row.cpp
        inc/dataframe/schema.h
        src/dataframe/schema.cpp
        inc/dataframe/dataframe.h
        src/dataframe/dataframe.cpp
        inc/dataframe/rower.h
        inc/dataframe/copy_writer.h
        src/dataframe/copy_writer.cpp
        inc/dataframe/sorer_integrator.h)
set(NETWORK_SRC
        inc/messages/serial.h
        src/messages/serial.cpp
        inc/network/distributed_app_network_layer.h
        src/network/distributed_app_network_layer.cpp
        inc/network/socket_network.h
        src/network/socket_network.cpp
        inc/network/recv_msg_manager.h
        inc/application/stdout_msg_manager.h
        src/application/stdout_msg_manager.cpp)
set(APP_SRC
        inc/application/key.h
        src/application/key.cpp
        inc/application/application.h
        src/application/application.cpp
        inc/application/key_value_store.h
        src/application/key_value_store.cpp
        inc/application/local_network_msg_manager.h
        src/application/local_network_msg_manager.cpp
        inc/application/application_network_interface.h
        src/application/application_network_interface.cpp
        inc/application/socket_network_msg_manager.h
        src/application/socket_network_msg_manager.cpp
        src/application/distributed_value.cpp
        inc/application/distributed_value.h
        inc/application/argparser.h
        src/application/argparser.cpp)

# Create targets for the test suites
add_executable(test_suite_dataframe
        tests/test_dataframe.cpp
        "${UTILITY_SRC}"
        "${DATAFRAME_SRC}")
add_executable(test_suite_messages
        tests/test_messages.cpp
        "${UTILITY_SRC}"
        inc/messages/serial.h
        src/messages/serial.cpp)
add_executable(test_suite_sorer
        tests/test_sorer.cpp
        "${UTILITY_SRC}"
        "${DATAFRAME_SRC}"
        src/dataframe/CS4500-A1-part1/column.h
        src/dataframe/CS4500-A1-part1/parser.h
        src/dataframe/CS4500-A1-part1/object.h)
add_executable(test_suite_kv_store
        tests/test_kv_store.cpp
        "${UTILITY_SRC}"
        "${DATAFRAME_SRC}"
        "${NETWORK_SRC}"
        "${APP_SRC}")

# Create targets to demo the applications without a real network layer
add_executable(demo_trivial_nonet
        tests/demo_trivial_nonet.cpp
        "${UTILITY_SRC}"
        "${DATAFRAME_SRC}"
        "${NETWORK_SRC}"
        "${APP_SRC}"
        inc/application/trivial_app.h
        src/application/trivial_app.cpp)
add_executable(demo_app_nonet
        tests/demo_app_nonet.cpp
        "${UTILITY_SRC}"
        "${DATAFRAME_SRC}"
        "${NETWORK_SRC}"
        "${APP_SRC}"
        inc/application/demo_app.h
        src/application/demo_app.cpp)
add_executable(demo_wordcount_nonet
        tests/demo_wordcount_nonet.cpp
        "${UTILITY_SRC}"
        "${DATAFRAME_SRC}"
        "${NETWORK_SRC}"
        "${APP_SRC}"
        inc/application/word_count_app.h
        src/application/word_count_app.cpp)
add_executable(demo_linus_nonet
        tests/demo_linus_nonet.cpp
        "${UTILITY_SRC}"
        "${DATAFRAME_SRC}"
        "${NETWORK_SRC}"
        "${APP_SRC}"
        inc/application/linus_app.h
        src/application/linus_app.cpp)

# Create targets to demo registrar/node functionality
add_executable(demo_registrar
        demos/demo_registrar.cpp
        "${UTILITY_SRC}"
        "${DATAFRAME_SRC}"
        "${NETWORK_SRC}")
add_executable(demo_node
        demos/demo_node.cpp
        "${UTILITY_SRC}"
        "${DATAFRAME_SRC}"
        "${NETWORK_SRC}")

# Creates targets to demo applications using real networks
add_executable(demo_app_registrar
        demos/demo_app_registrar.cpp
        "${UTILITY_SRC}"
        "${DATAFRAME_SRC}"
        "${NETWORK_SRC}"
        "${APP_SRC}"
        inc/application/demo_app.h
        src/application/demo_app.cpp)
add_executable(demo_app_node
        demos/demo_app_node.cpp
        "${UTILITY_SRC}"
        "${DATAFRAME_SRC}"
        "${NETWORK_SRC}"
        "${APP_SRC}"
        inc/application/demo_app.h
        src/application/demo_app.cpp)

add_executable(demo_wordcount_registrar
        demos/demo_wordcount_registrar.cpp
        "${UTILITY_SRC}"
        "${DATAFRAME_SRC}"
        "${NETWORK_SRC}"
        "${APP_SRC}"
        inc/application/word_count_app.h
        src/application/word_count_app.cpp)
add_executable(demo_wordcount_node
        demos/demo_wordcount_node.cpp
        "${UTILITY_SRC}"
        "${DATAFRAME_SRC}"
        "${NETWORK_SRC}"
        "${APP_SRC}"
        inc/application/word_count_app.h
        src/application/word_count_app.cpp)

add_executable(demo_linus_registrar
        demos/demo_linus_registrar.cpp
        "${UTILITY_SRC}"
        "${DATAFRAME_SRC}"
        "${NETWORK_SRC}"
        "${APP_SRC}"
        inc/application/linus_app.h
        src/application/linus_app.cpp)
add_executable(demo_linus_node
        demos/demo_linus_node.cpp
        "${UTILITY_SRC}"
        "${DATAFRAME_SRC}"
        "${NETWORK_SRC}"
        "${APP_SRC}"
        inc/application/linus_app.h
        src/application/linus_app.cpp)
