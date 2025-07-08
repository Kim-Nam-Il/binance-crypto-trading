cmake_minimum_required(VERSION 3.16)
project(BinanceTrader)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(CURL REQUIRED libcurl)
find_package(OpenSSL REQUIRED)

# Add executable
add_executable(binance_trader 
    src/main.cpp 
    src/binance_api.cpp
    src/json_parser.cpp
    src/secure_storage.cpp
)

# Include directories
target_include_directories(binance_trader PRIVATE include)

# Link libraries
target_link_libraries(binance_trader 
    ${CURL_LIBRARIES} 
    OpenSSL::SSL 
    OpenSSL::Crypto
)

target_compile_options(binance_trader PRIVATE ${CURL_CFLAGS_OTHER}) 