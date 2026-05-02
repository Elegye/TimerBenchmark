#include <benchmark/benchmark.h>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <cstring>
#include <time.h>

#include "benchmark.hpp"


// --- Vector (swap-and-pop) ---
struct VectorSchedulerEraseIf {
    struct Entry { uint32_t id; FakeTimer timer; };
    std::vector<Entry> entries;
    uint32_t next_id = 1;

    explicit VectorSchedulerEraseIf(size_t capacity) {
        entries.reserve(capacity);
    }

    TimerID schedule(const std::chrono::seconds& delay, ITimeout& /** unused */) {
        uint32_t id = next_id++;
        entries.push_back({id, FakeTimer{}});
        return {id};
    }

    void unschedule(const TimerID& tid) {
        entries.erase(
            std::remove_if(entries.begin(), entries.end(),
                [&tid](const Entry& e) { return e.id == tid.id; }), entries.end());
    }
};

REGISTER(BM_Unschedule_Cold, VectorSchedulerEraseIf);
REGISTER(BM_Schedule_Cold, VectorSchedulerEraseIf);
REGISTER(BM_Unschedule_Hot,  VectorSchedulerEraseIf);
REGISTER(BM_Schedule_Hot, VectorSchedulerEraseIf);

BENCHMARK_MAIN();