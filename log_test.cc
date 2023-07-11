#if defined(__GNUC__)
#define BENCHMARK_NOINLINE __attribute__((noinline))
#else
#define BENCHMARK_NOINLINE
#endif
#include <sys/stat.h>
#include <fcntl.h>
#include <benchmark/benchmark.h>
#include <random>
#include <array>
#include <string>
#include <string_view>
#include <fstream>
#include <iostream>
#include <glog/logging.h>
#include <glog/raw_logging.h>
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#define SPDLOG_ST_LOGGER "spdlog_st"
#define SPDLOG_ASYNC_LOGGER "spdlog_async"
#define SPDLOG_FMT_PATTERN "%L%Y%m%d %H:%M:%S.%f %t %s:%#] %v"
namespace test
{
    void BENCHMARK_NOINLINE log_by_glog(const std::string_view &msg)
    {
        LOG(INFO) << msg;
    }
    void BENCHMARK_NOINLINE log_by_glog_fmt(const char *str, unsigned long l, double d)
    {
        LOG(INFO) << __FUNCTION__ << ": str=" << str << ", l=" << l << ", d=" << d;
    }
    void BENCHMARK_NOINLINE log_by_spdlog_fmt(const char *str, unsigned long l, double d)
    {
        SPDLOG_INFO("{}: str={}, l={}, d={}", __FUNCTION__, str, l, d);
    }

    void BENCHMARK_NOINLINE log_by_spdlog(const std::string_view &msg)
    {
        SPDLOG_INFO(msg);
    }
    // log by spdlog with basic_logger_st
    void BENCHMARK_NOINLINE log_by_spdlog_st(const std::string_view &msg)
    {
        SPDLOG_LOGGER_WARN(spdlog::get(SPDLOG_ST_LOGGER), msg);
    }
    void BENCHMARK_NOINLINE log_by_spdlog_async(const std::string_view &msg)
    {
        SPDLOG_LOGGER_WARN(spdlog::get(SPDLOG_ASYNC_LOGGER), msg);
    }
    void dosetup_glog(const benchmark::State &state)
    {
        google::InitGoogleLogging("glog");
        FLAGS_log_dir = "./test";

        auto size = state.range(0);
        auto file_name = spdlog::fmt_lib::format("./test/glog_{}_{}.log",
                                                 state.name().find("st") != std::string::npos ? "st" : "mt",
                                                 size);
        struct stat st;
        if (stat(file_name.c_str(), &st) == 0)
        {
            remove(file_name.c_str());
        }
        google::SetLogDestination(google::GLOG_INFO, file_name.c_str());
        google::SetLogSymlink(google::GLOG_INFO, "");
        google::InstallFailureSignalHandler();
        FLAGS_timestamp_in_logfile_name = false;
    }
    void dosetup_spdlog(const benchmark::State &state)
    {
        auto size = state.range(0);
        auto file_name = spdlog::fmt_lib::format("./test/spdlog_mt_{}.log", size);
        auto logger = spdlog::basic_logger_mt("spdlog", file_name, true);
        logger->set_level(spdlog::level::info);
        logger->set_pattern(SPDLOG_FMT_PATTERN);
        spdlog::set_default_logger(logger);
    }
    void doteardown_glog(const benchmark::State &state)
    {
        if (google::IsGoogleLoggingInitialized())
        {
            google::ShutdownGoogleLogging();
        }
    }
    void doteardown_spdlog(const benchmark::State &_)
    {
        spdlog::shutdown();
    }

    void dosetup_spdlog_st(const benchmark::State &state)
    {
        if (spdlog::get(SPDLOG_ST_LOGGER).get() != nullptr)
        {
            spdlog::drop(SPDLOG_ST_LOGGER);
        }
        auto size = state.range(0);
        auto file_name = spdlog::fmt_lib::format("./test/spdlog_st_{}.log", size);
        auto logger = spdlog::basic_logger_st(SPDLOG_ST_LOGGER, file_name, true);
        logger->set_level(spdlog::level::info);
        logger->set_pattern(SPDLOG_FMT_PATTERN);
        spdlog::set_default_logger(logger);
    }
    void doteardown_spdlog_st(const benchmark::State &_)
    {
        spdlog::drop(SPDLOG_ST_LOGGER);
    }
    void dosetup_spdlog_async(const benchmark::State &state)
    {
        if (spdlog::get(SPDLOG_ASYNC_LOGGER).get() != nullptr)
        {
            spdlog::drop(SPDLOG_ASYNC_LOGGER);
        }
        auto size = state.range(0);
        auto file_name = spdlog::fmt_lib::format("./test/spdlog_async_{}.log", size);
        spdlog::init_thread_pool(8192, 1);
        auto logger = spdlog::basic_logger_mt<spdlog::async_factory>(SPDLOG_ASYNC_LOGGER, file_name, true);
        logger->set_level(spdlog::level::info);
        logger->set_pattern(SPDLOG_FMT_PATTERN);
        spdlog::set_default_logger(logger);
    }
    void doteardown_spdlog_async(const benchmark::State &_)
    {
        spdlog::shutdown();
    }
    static constexpr char kChars[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    static constexpr size_t kCharsSize = sizeof(kChars) - 1;
    // generate random uint64
    inline static std::mt19937_64 random_engine()
    {
        std::random_device source;
        auto constexpr bytes = std::mt19937_64::state_size * sizeof(std::mt19937_64::result_type);
        auto constexpr seed_len = bytes / sizeof(std::seed_seq::result_type);
        std::array<std::seed_seq::result_type, seed_len> seed;
        std::generate_n(std::begin(seed), seed_len, std::ref(source));
        auto seed_seq = std::seed_seq(std::begin(seed), std::end(seed));
        return std::mt19937_64(seed_seq);
    }
    void fill_log_msg(char *buf, size_t size)
    {
        static auto random = random_engine();
        for (size_t i = 0, n = size / 8; i < n; i++)
        {
            auto r = random();
            memccpy(buf + i * 8, &r, 8, 8);
        }

        auto remain = size % 8;
        if (remain > 0)
        {
            auto r = random();
            memccpy(buf + size - remain, &r, remain, remain);
        }
        for (size_t i = 0; i < size; i++)
        {
            auto *p = buf + i;
            *p = kChars[static_cast<int>(*p) % kCharsSize];
        }
        buf[size] = '\0';
    }
    // benchamrk log
    template <typename Func>
    static void BM_Log(benchmark::State &state, Func &&func)
    {
        auto log = std::forward<Func>(func);
        auto size = state.range(0);
        auto *buf = new char[size + 1];
        size_t bytes = 0;
        for (auto _ : state)
        {
            state.PauseTiming();
            fill_log_msg(buf, size);
            std::string_view msg(buf, size);
            bytes += size;
            state.ResumeTiming();
            std::invoke(log, msg);
        }
        delete[] buf;
        state.SetItemsProcessed(state.iterations());
        state.SetBytesProcessed(bytes);
    }

    template <typename Func>
    static void BM_Log_FMT(benchmark::State &state, Func &&func)
    {
        static auto random = random_engine();
        auto size = state.range(0);
        char buf[size + 1];
        unsigned long l;
        double d;
        for (auto _ : state)
        {
            state.PauseTiming();
            fill_log_msg(buf, size);
            l = random();
            d = static_cast<double>(random()) / random.max();
            state.ResumeTiming();
            func(buf, l, d);
        }
        state.SetItemsProcessed(state.iterations());
    }
#define BM_LOG_ST(log)                                     \
    BENCHMARK_CAPTURE(BM_Log, log##_st, log_by_##log##_st) \
        ->Arg(1 << 10)                                     \
        ->Arg(1 << 12)                                     \
        ->Setup(dosetup_##log##_st)                        \
        ->Teardown(doteardown_##log##_st)
#define BM_LOG_MT(log)                                \
    BENCHMARK_CAPTURE(BM_Log, log##_mt, log_by_##log) \
        ->Threads(1)                                  \
        ->Threads(4)                                  \
        ->Threads(6)                                  \
        ->Arg(1 << 10)                                \
        ->Setup(dosetup_##log)                        \
        ->Teardown(doteardown_##log)
#define BM_LOG_FMT(log)                                          \
    BENCHMARK_CAPTURE(BM_Log_FMT, log##_fmt, log_by_##log##_fmt) \
        ->Arg(1 << 10)                                           \
        ->Threads(1)                                             \
        ->Threads(4)                                             \
        ->Setup(dosetup_##log)                                   \
        ->Teardown(doteardown_##log)

    BM_LOG_ST(spdlog);
    BM_LOG_MT(glog);
    BM_LOG_MT(spdlog);
    BM_LOG_MT(spdlog_async);
    BM_LOG_FMT(glog);
    BM_LOG_FMT(spdlog);
#undef BM_LOG_ST
#undef BM_LOG_MT
#undef BM_LOG_FMT
}

BENCHMARK_MAIN();