cmake_minimum_required(VERSION 3.14)
project(ozanCppDemo VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(demo
    src/main.cpp
    src/auth/auth_manager.cpp
    src/network/http_server.cpp
    src/database/db_handler.cpp
    src/utils/string_utils.cpp
    src/utils/file_utils.cpp
    src/crypto/crypto_utils.cpp
    src/third_party/gpl_library.cpp
)

target_include_directories(demo PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

add_executable(tests
    tests/test_main.cpp
    src/auth/auth_manager.cpp
    src/database/db_handler.cpp
    src/utils/string_utils.cpp
    src/utils/file_utils.cpp
    src/crypto/crypto_utils.cpp
    src/third_party/gpl_library.cpp
)

target_include_directories(tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
