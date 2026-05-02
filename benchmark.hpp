#include <benchmark/benchmark.h>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <cstring>
#include <time.h>

// Cache Pollution : un tableau suffisamment grand pour écraser les données en cache
static constexpr size_t L1_SIZE       = 64 * 1024;
static constexpr size_t POLLUTION_SIZE = 2 * L1_SIZE;

static volatile uint8_t cache_trash[POLLUTION_SIZE];

static void pollute_cache() {
    for (size_t i = 0; i < sizeof(cache_trash); i += 64)
        cache_trash[i] = static_cast<uint8_t>(i);
}

static double monotonic_raw_seconds() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return static_cast<double>(ts.tv_sec)
         + static_cast<double>(ts.tv_nsec) * 1e-9;
}

// Under-test structures
struct TimerID { uint32_t id; };
// A shared_ptr consists of two pointer:
// control block and data pointer, on 64-bit platforms this is 16 bytes total.
struct FakeTimer {
    void* ctrl  = nullptr;
    void* ptr   = nullptr;
};

struct ITimeout {
    virtual ~ITimeout() = default;
    virtual void onTimeout(const TimerID&) = 0;
};

class DummyCallback : public ITimeout {
    inline void onTimeout(const TimerID&) override {}
};

// Benchmark utililities    
template<typename Scheduler>
static std::vector<TimerID> populate(Scheduler& s, size_t n) {
    DummyCallback callback;
    std::vector<TimerID> ids;
    ids.reserve(n);
    for (size_t i = 0; i < n; ++i)
        ids.push_back(s.schedule(std::chrono::seconds(1), callback));

    // Mélange déterministe (Fisher-Yates avec seed fixe)
    uint32_t rng = 0xdeadbeef;
    for (size_t i = ids.size() - 1; i > 0; --i) {
        rng ^= rng << 13; rng ^= rng >> 17; rng ^= rng << 5; // xorshift
        size_t j = rng % (i + 1);
        std::swap(ids[i], ids[j]);
    }
    return ids;
}

// Benchmarks
template<typename Scheduler>
static void BM_Unschedule_Cold(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));

    for (auto _ : state) {
        // Setup hors mesure
        state.PauseTiming();
        Scheduler sched(n);
        auto ids = populate(sched, n);
        pollute_cache();                        // accès à froid
        state.ResumeTiming();

        // Mesure manuelle avec CLOCK_MONOTONIC_RAW
        double t0 = monotonic_raw_seconds();
        for (auto& id : ids)
            sched.unschedule(id);
        double t1 = monotonic_raw_seconds();

        state.SetIterationTime(t1 - t0);        // injecte notre mesure
    }

    state.SetComplexityN(static_cast<int64_t>(n));
}

template<typename Scheduler>
static void BM_Unschedule_Hot(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    for (auto _ : state) {
        state.PauseTiming();
        Scheduler sched(n);
        auto ids = populate(sched, n);

        // Warmup : on touche les données pour les amener en cache
        benchmark::DoNotOptimize(sched);
        benchmark::ClobberMemory();
        state.ResumeTiming();

        double t0 = monotonic_raw_seconds();
        for (auto& id : ids)
            sched.unschedule(id);
        double t1 = monotonic_raw_seconds();

        state.SetIterationTime(t1 - t0);
    }

    state.SetComplexityN(static_cast<int64_t>(n));
}

template<typename Scheduler>
static void BM_Schedule_Cold(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));

    for (auto _ : state) {
        // Setup hors mesure
        state.PauseTiming();
        DummyCallback callback;
        Scheduler sched(n);
        auto ids = populate(sched, n);
        pollute_cache();                        // accès à froid
        state.ResumeTiming();

        // Mesure manuelle avec CLOCK_MONOTONIC_RAW
        double t0 = monotonic_raw_seconds();
        for (auto& id : ids)
            auto __ = sched.schedule(std::chrono::seconds(1), callback);
        double t1 = monotonic_raw_seconds();

        state.SetIterationTime(t1 - t0);        // injecte notre mesure
    }

    state.SetComplexityN(static_cast<int64_t>(n));
}

template<typename Scheduler>
static void BM_Schedule_Hot(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    for (auto _ : state) {
        state.PauseTiming();
        Scheduler sched(n);
        DummyCallback callback;
        auto ids = populate(sched, n);

        // Warmup : on touche les données pour les amener en cache
        benchmark::DoNotOptimize(sched);
        benchmark::ClobberMemory();
        state.ResumeTiming();

        double t0 = monotonic_raw_seconds();
        for (auto& id : ids)
            auto __ = sched.schedule(std::chrono::seconds(1), callback);
        double t1 = monotonic_raw_seconds();

        state.SetIterationTime(t1 - t0);
    }

    state.SetComplexityN(static_cast<int64_t>(n));
}

// 2 to 2^16 timers
#define REGISTER(name, sched)                                    \
    BENCHMARK_TEMPLATE(name, sched)                              \
        ->RangeMultiplier(2)->Range(1, 2048)                     \
        ->UseManualTime()                                        \
        ->ReportAggregatesOnly(true)                             \
        ->Unit(benchmark::kNanosecond)