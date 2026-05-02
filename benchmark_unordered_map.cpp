#include <benchmark/benchmark.h>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <cstring>
#include <time.h>
#include "benchmark.hpp"

struct UnorderedMapScheduler {
    std::unordered_map<uint32_t, FakeTimer> map;
    uint32_t next_id = 1;

    explicit UnorderedMapScheduler(size_t capacity) {
        map.reserve(capacity);
    }

    TimerID schedule(const std::chrono::seconds& delay, ITimeout& /** unused */) {
        uint32_t id = next_id++;
        map.emplace(id, FakeTimer{});
        return {id};
    }

    void unschedule(const TimerID& tid) {
        map.erase(tid.id);
    }
};

REGISTER(BM_Unschedule_Cold, UnorderedMapScheduler);
REGISTER(BM_Schedule_Cold, UnorderedMapScheduler);
REGISTER(BM_Unschedule_Hot,  UnorderedMapScheduler);
REGISTER(BM_Schedule_Hot, UnorderedMapScheduler);

BENCHMARK_MAIN();