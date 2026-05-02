#include <benchmark/benchmark.h>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <cstring>
#include <time.h>

#include "benchmark.hpp"


// --- Vector (swap-and-pop) ---
struct VectorSchedulerSwap {
    struct Entry { uint32_t id; FakeTimer timer; };
    std::vector<Entry> entries;
    uint32_t next_id = 1;

    explicit VectorSchedulerSwap(size_t capacity) {
        entries.reserve(capacity);
    }

    TimerID schedule(const std::chrono::seconds& delay, ITimeout& /** unused */) {
        uint32_t id = next_id++;
        entries.push_back({id, FakeTimer{}});
        return {id};
    }

    void unschedule(const TimerID& tid) {
        for (size_t i = 0; i < entries.size(); ++i) {
            if (entries[i].id == tid.id) {
                entries[i] = std::move(entries.back());
                entries.pop_back();
                return;
            }
        }
    }
};

REGISTER(BM_Unschedule_Cold, VectorSchedulerSwap);
REGISTER(BM_Schedule_Cold, VectorSchedulerSwap);
REGISTER(BM_Unschedule_Hot,  VectorSchedulerSwap);
REGISTER(BM_Schedule_Hot, VectorSchedulerSwap);

BENCHMARK_MAIN();