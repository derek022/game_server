cmake_minimum_required(VERSION 3.0)
project(sylar)

include (cmake/utils.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function  -Wno-builtin-macro-redefined -Wno-unused-result")
set(CMAKE_C_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -ggdb -std=c11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-unused-result")
include_directories(.)

option(BUILD_TEST "ON for complile test" OFF)

find_package(Boost REQUIRED)
if(Boost_FOUND)
        include_directories(${Boost_INCLUDE_DIRS})
endif()

find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIR})
endif()

find_package(ZLIB REQUIRED)
if(ZLIB_FOUND)
    include_directories(${ZLIB_INCLUDE_DIR})
endif()

find_package(Protobuf REQUIRED)
if(Protobuf_FOUND)
    include_directories(${Protobuf_INCLUDE_DIRS})
endif()

find_library(YAMLCPP yaml-cpp)

set(LIB_SRC 
    sylar/log.cc
    sylar/util.cc
    sylar/thread.cc
    sylar/config.cc
    sylar/mutex.cc
    sylar/fiber.cc
    sylar/scheduler.cc
    sylar/iomanager.cc
    sylar/timer.cc
    sylar/hook.cc
    sylar/fd_manager.cc
    sylar/address.cc
    sylar/socket.cc
    sylar/worker.cc
    sylar/application.cc
    sylar/library.cc
    sylar/module.cc
    sylar/daemon.cc
    sylar/env.cc
    sylar/util/hash_util.cc
    sylar/util/json_util.cc
    sylar/util/crypto_util.cc
    sylar/http/ws_session.cc
    sylar/http/ws_server.cc
    sylar/http/ws_servlet.cc
    sylar/http/ws_connection.cc
    sylar/http/session_data.cc
    sylar/bytearray.cc
    sylar/http/http.cc
    sylar/http/http_parser.cc
    sylar/http/http_session.cc
    sylar/http/servlet.cc
    sylar/http/http_server.cc
    sylar/http/http_connection.cc
    sylar/http/servlets/config_servlet.cc
    sylar/http/servlets/status_servlet.cc
    sylar/streams/zlib_stream.cc
    sylar/streams/socket_stream.cc
    sylar/streams/async_socket_stream.cc
    sylar/streams/load_balance.cc
    sylar/tcp_server.cc
    sylar/stream.cc
    sylar/protocol.cc
    sylar/rock/rock_protocol.cc
    sylar/rock/rock_stream.cc
    sylar/rock/rock_server.cc
    sylar/db/sqlite3.cc
    sylar/db/redis.cc
    sylar/db/fox_thread.cc
    sylar/ds/bitmap.cc
    sylar/ds/roaring_bitmap.cc
    sylar/ds/roaring.c
    sylar/email/email.cc
    sylar/email/smtp.cc
    sylar/ns/name_server_module.cc
    sylar/ns/ns_client.cc
    sylar/ns/ns_protocol.cc
    )


ragelmaker(sylar/http/http11_parser.rl LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sylar/http)
ragelmaker(sylar/http/httpclient_parser.rl LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sylar/http)
ragelmaker(sylar/uri.rl LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sylar)

protobufmaker(sylar/ns/ns_protobuf.proto LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(sylar SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(sylar)

set(LIBS  
        sylar 
        dl 
        ${ZLIB_LIBRARIES}
        pthread 
        jsoncpp
        sqlite3
        tinyxml2
        ${OPENSSL_LIBRARIES}
        ${PROTOBUF_LIBRARIES}
        jemalloc
        event
        hiredis_vip
        ${YAMLCPP})



if(BUILD_TEST)

sylar_add_executable(test "tests/test.cc" sylar "${LIBS}")
sylar_add_executable(test_config "tests/test_config.cc" sylar "${LIBS}")
sylar_add_executable(test_thread "tests/test_thread.cc" sylar "${LIBS}")
sylar_add_executable(test_util "tests/test_util.cc" sylar "${LIBS}")
sylar_add_executable(test_fiber "tests/test_fiber.cc" sylar "${LIBS}")
sylar_add_executable(test_scheduler "tests/test_scheduler.cc" sylar "${LIBS}")
sylar_add_executable(test_iomanager "tests/test_iomanager.cc" sylar "${LIBS}")
sylar_add_executable(test_hook "tests/test_hook.cc" sylar "${LIBS}")

sylar_add_executable(test_address "tests/test_address.cc" sylar "${LIBS}")
sylar_add_executable(test_socket "tests/test_socket.cc" sylar "${LIBS}")
sylar_add_executable(test_bytearray "tests/test_bytearray.cc" sylar "${LIBS}")
sylar_add_executable(test_http "tests/test_http.cc" sylar "${LIBS}")
sylar_add_executable(test_http_parser "tests/test_http_parser.cc" sylar "${LIBS}")

sylar_add_executable(test_echo_server "tests/test_echo_server.cc" sylar "${LIBS}")
sylar_add_executable(test_http_server "tests/test_http_server.cc" sylar "${LIBS}")
sylar_add_executable(test_http_connection "tests/test_http_connection.cc" sylar "${LIBS}")

sylar_add_executable(test_daemon "tests/test_daemon.cc" sylar "${LIBS}")
sylar_add_executable(test_env "tests/test_env.cc" sylar "${LIBS}")
sylar_add_executable(test_hash "tests/test_hash.cc" sylar "${LIBS}")
sylar_add_executable(test_openssl "tests/test_openssl.cc" sylar "${LIBS}")
sylar_add_executable(test_ws_server "tests/test_ws_server.cc" sylar "${LIBS}")
sylar_add_executable(test_ws_client "tests/test_ws_client.cc" sylar "${LIBS}")
sylar_add_executable(test_crypto "tests/test_crypto.cc" sylar "${LIBS}")

sylar_add_executable(test_sqlite "tests/test_sqlite3.cc" sylar "${LIBS}")
sylar_add_executable(test_rock "tests/test_rock.cc" sylar "${LIBS}")
sylar_add_executable(test_nameserver "tests/test_nameserver.cc" sylar "${LIBS}")
endif()

sylar_add_executable(test_bitmap "tests/test_bitmap.cc" sylar "${LIBS}")

set(ORM_SRCS
    sylar/orm/table.cc
    sylar/orm/column.cc
    sylar/orm/index.cc
    sylar/orm/orm.cc
    sylar/orm/util.cc
    )
sylar_add_executable(orm "${ORM_SRCS}" sylar "${LIBS}")

sylar_add_executable(bin_sylar "sylar/main.cc" sylar "${LIBS}")
set_target_properties(bin_sylar PROPERTIES OUTPUT_NAME "sylar")


# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/orm_out)
# set(OLIBS ${LIBS})
# set(OLIBS ${OLIBS} orm_data)
# sylar_add_executable(test_orm "tests/test_orm.cc" orm_data "${OLIBS}")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
