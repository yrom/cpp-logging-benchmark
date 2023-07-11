cmake_minimum_required(VERSION 3.20)

project(logs-benchmark CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
# install external library from https://github.com/google/benchmark/archive/v1.8.2.tar.gz
FetchContent_Declare(
    benchmark
    URL https://github.com/google/benchmark/archive/v1.8.2.tar.gz
    URL_HASH SHA256=2aab2980d0376137f969d92848fbb68216abb07633034534fc8c65cc4e7a0e93
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_Declare(
    glog
    URL https://github.com/google/glog/archive/v0.6.0.zip
    URL_HASH SHA256=122fb6b712808ef43fbf80f75c52a21c9760683dae470154f02bddfc61135022
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    FIND_PACKAGE_ARGS
)
FetchContent_Declare(
    spdlog
    URL https://github.com/gabime/spdlog/archive/refs/tags/v1.12.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    FIND_PACKAGE_ARGS
)

set(BENCHMARK_ENABLE_TESTING OFF CACHE INTERNAL "")
set(BENCHMARK_ENABLE_INSTALL OFF CACHE INTERNAL "")
set(BENCHMARK_INSTALL_DOCS OFF CACHE INTERNAL "")
if(MINGW)
    set(BENCHMARK_ENABLE_WERROR OFF CACHE INTERNAL "")
endif(MINGW)
FetchContent_MakeAvailable(benchmark)
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
set(WITH_GFLAGS OFF CACHE INTERNAL "")
set(WITH_SYMBOLIZE OFF CACHE INTERNAL "")
set(WITH_UNWIND OFF CACHE INTERNAL "")
set(WITH_GTEST OFF CACHE INTERNAL "")
set(WITH_PKGCONFIG OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(glog)

set(SPDLOG_INSTALL OFF CACHE INTERNAL "")
set(SPDLOG_BUILD_SHARED OFF CACHE INTERNAL "")
set(SPDLOG_BUILD_EXAMPLE OFF CACHE INTERNAL "")
if (CMAKE_CROSSCOMPILING)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
    set(SPDLOG_NO_EXCEPTIONS ON CACHE INTERNAL "")
endif(CMAKE_CROSSCOMPILING)

FetchContent_MakeAvailable(spdlog)
add_executable(log_test log_test.cc)
target_link_libraries(log_test PRIVATE benchmark::benchmark glog::glog spdlog::spdlog_header_only)

if(ANDROID)
    add_custom_target(run_benchmark 
        COMMAND adb devices -l
        COMMAND adb push log_test /data/local/tmp/
        COMMAND adb shell chmod 777 /data/local/tmp/log_test
        COMMAND adb shell \"cd /data/local/tmp\; test -d test && rm -r ./test || true\; mkdir ./test\; ./log_test --benchmark_min_time=0.3s --benchmark_counters_tabular=true\"
        DEPENDS log_test
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
elseif(WIN32)
    add_custom_target(run_benchmark 
        COMMAND IF exist test ( rmdir /s /q test )
        COMMAND mkdir test
        COMMAND log_test --benchmark_min_time=1s --benchmark_counters_tabular=true
        DEPENDS log_test
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
else()
    add_custom_target(run_benchmark 
        COMMAND test -d ./test && rm -rf ./test || true
        COMMAND mkdir -p ./test
        COMMAND log_test --benchmark_min_time=1s --benchmark_counters_tabular=true
        DEPENDS log_test
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()