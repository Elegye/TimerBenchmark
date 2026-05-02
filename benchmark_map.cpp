#include <benchmark/benchmark.h>
#include <vector>
#include <map>
#include <cstdint>
#include <cstring>
#include <time.h>
#include "benchmark.hpp"

struct MapScheduler {
    std::map<uint32_t, FakeTimer> map;
    uint32_t next_id = 1;

    explicit MapScheduler(size_t capacity) {

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

REGISTER(BM_Unschedule_Cold, MapScheduler);
REGISTER(BM_Schedule_Cold, MapScheduler);
REGISTER(BM_Unschedule_Hot,  MapScheduler);
REGISTER(BM_Schedule_Hot, MapScheduler);

BENCHMARK_MAIN();