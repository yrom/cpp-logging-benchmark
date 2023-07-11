## Benchmark for Portable C++ Logging Libraries

Currently this project tests the following libraries:

1. [spdlog][spdlog]
2. [glog][glog]

Benchmarking rule set:

0. logging message in google logging prefix format (`\[IWEF\]yyyymmdd hh:mm:ss.uuuuuu threadid file:line\] msg`)
1. single-thread 
2. multi-thread
3. async logging
4. formatted (or placeholder) log message

Notes:

- The glog does not support _single-thread_ and _async logging_
- APIs for formatting log messages differ between libraries

### Run benchmark

Run on macOS: 

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build ./build --config Release --target run_benchmark
```

Run on Windows (by [Mingw-w64 toolchain on MSYS2](https://packages.msys2.org/groups/mingw-w64-x86_64-toolchain)): 

```sh
cmake.exe -DCMAKE_C_COMPILER:FILEPATH=C:\msys64\mingw64\bin\gcc.exe -DCMAKE_CXX_COMPILER:FILEPATH=C:\msys64\mingw64\bin\g++.exe -B build -DCMAKE_BUILD_TYPE=Release
cmake.exe --build build --config Release --target run_benchmark
```

Run on Android arm64 device:

```sh
cmake -B android-build -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
    -DCMAKE_ANDROID_NDK=$ANDROID_NDK_HOME -DCMAKE_SYSTEM_NAME=Android \
    -DANDROID_PLATFORM=android-21 -DANDROID_ABI=arm64-v8a

#... after connecting Android phone via USB
cmake --build ./android-build --target run_benchmark
```

### Benchmark result

MacBook Pro (15-inch, 2018)

    Run on (12 X 2600 MHz CPU s)
    CPU Caches:
    L1 Data 32 KiB
    L1 Instruction 32 KiB
    L2 Unified 256 KiB (x6)
    L3 Unified 9216 KiB
    Load Average: 2.59, 2.71, 2.67
    ------------------------------------------------------------------------------------------------------------------
    Benchmark                                      Time             CPU   Iterations bytes_per_second items_per_second
    ------------------------------------------------------------------------------------------------------------------
    BM_Log/spdlog_st/1024                       3415 ns         3291 ns       432532       296.699M/s        303.82k/s
    BM_Log/spdlog_st/4096                       7646 ns         7251 ns       162595       538.712M/s        137.91k/s
    BM_Log/glog_mt/1024/threads:1               9934 ns         9014 ns       136672       108.342M/s       110.942k/s
    BM_Log/glog_mt/1024/threads:4               7613 ns        14491 ns        99896       67.3897M/s       69.0071k/s
    BM_Log/glog_mt/1024/threads:6               8022 ns        15763 ns        90672       61.9521M/s        63.439k/s
    BM_Log/spdlog_mt/1024/threads:1             3232 ns         3129 ns       433094       312.099M/s        319.59k/s
    BM_Log/spdlog_mt/1024/threads:4             4687 ns         7873 ns       177616       124.042M/s       127.019k/s
    BM_Log/spdlog_mt/1024/threads:6             3179 ns         7237 ns       188736       134.937M/s       138.176k/s
    BM_Log/spdlog_async_mt/1024/threads:1       2466 ns         2425 ns       658594       402.726M/s       412.391k/s
    BM_Log/spdlog_async_mt/1024/threads:4        670 ns         2455 ns       559884       397.724M/s       407.269k/s
    BM_Log/spdlog_async_mt/1024/threads:6        597 ns         2842 ns       439686       343.635M/s       351.883k/s
    -------------------------------------------------------------------------------------------------
    Benchmark                                      Time             CPU   Iterations items_per_second
    -------------------------------------------------------------------------------------------------
    BM_Log_FMT/glog_fmt/1024/threads:1          9803 ns         9697 ns       130753       103.121k/s
    BM_Log_FMT/glog_fmt/1024/threads:4          6988 ns        15587 ns        90660       64.1567k/s
    BM_Log_FMT/spdlog_fmt/1024/threads:1        3996 ns         3874 ns       351808       258.101k/s
    BM_Log_FMT/spdlog_fmt/1024/threads:4        3241 ns         7502 ns       185024        133.29k/s


Vivo Y77 (model:V2219A, Android 12)

    Run on (8 X 2000 MHz CPU s)
    ***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
    ------------------------------------------------------------------------------------------------------------------
    Benchmark                                      Time             CPU   Iterations bytes_per_second items_per_second
    ------------------------------------------------------------------------------------------------------------------
    BM_Log/spdlog_st/1024                       2519 ns         2500 ns       160760       390.611M/s       399.985k/s
    BM_Log/spdlog_st/4096                       6640 ns         6500 ns        70431       600.926M/s       153.837k/s
    BM_Log/glog_mt/1024/threads:1              10780 ns        10563 ns        42749       92.4475M/s       94.6663k/s
    BM_Log/glog_mt/1024/threads:4              60212 ns       147415 ns         2700       6.62456M/s       6.78355k/s
    BM_Log/glog_mt/1024/threads:6             113007 ns       363736 ns         1758       2.68481M/s       2.74925k/s
    BM_Log/spdlog_mt/1024/threads:1             2417 ns         2382 ns       181982       409.899M/s       419.737k/s
    BM_Log/spdlog_mt/1024/threads:4            69655 ns       140792 ns         5432        6.9362M/s       7.10266k/s
    BM_Log/spdlog_mt/1024/threads:6            75188 ns       169731 ns         2670       5.75357M/s       5.89166k/s
    BM_Log/spdlog_async_mt/1024/threads:1       1703 ns         1661 ns       254253        587.99M/s       602.102k/s
    BM_Log/spdlog_async_mt/1024/threads:4       1421 ns         3624 ns       108964       269.453M/s        275.92k/s
    BM_Log/spdlog_async_mt/1024/threads:6       2031 ns         5178 ns        60000       188.605M/s       193.131k/s
    -------------------------------------------------------------------------------------------------
    Benchmark                                      Time             CPU   Iterations items_per_second
    -------------------------------------------------------------------------------------------------
    BM_Log_FMT/glog_fmt/1024/threads:1         11804 ns        11566 ns        37387        86.457k/s
    BM_Log_FMT/glog_fmt/1024/threads:4         63850 ns       176122 ns         3212       5.67789k/s
    BM_Log_FMT/spdlog_fmt/1024/threads:1        2971 ns         2909 ns       147192       343.764k/s
    BM_Log_FMT/spdlog_fmt/1024/threads:4       97157 ns       183122 ns         5120       5.46083k/s

[spdlog]: https://github.com/gabime/spdlog
[glog]: https://github.com/google/glog