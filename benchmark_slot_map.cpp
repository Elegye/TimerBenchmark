#include <benchmark/benchmark.h>
#include <vector>
#include <map>
#include <cstdint>
#include <cstring>
#include <time.h>
#include "benchmark.hpp"

struct SlotMapScheduler {
    struct Slot {
        uint32_t id;
        FakeTimer timer;
    };

    std::vector<Slot> _slots;
    std::vector<uint32_t> _index;

    uint32_t next_id = 1;

    explicit SlotMapScheduler(size_t capacity) {
        _slots.reserve(capacity);
        _index.resize(capacity, UINT32_MAX);
    }

    TimerID schedule(const std::chrono::seconds& delay, ITimeout& /** unused */) {
        uint32_t id = next_id++;
        if (id >= _index.size())
            _index.resize(id + 1, UINT32_MAX);

        _index[id] = static_cast<uint32_t>(_slots.size());
        _slots.push_back({id, FakeTimer{}});

        return {id};
    }

    void unschedule(const TimerID& tid) {
        uint32_t idx = _index[tid.id];
        uint32_t last_idx = _slots.back().id;

        _slots[idx] = std::move(_slots.back()); // Move last slot to the freed position
        _index[last_idx] = idx; // Update the index of the moved slot
        _slots.pop_back(); // Remove the last slot
        _index[tid.id] = UINT32_MAX; // Mark the ID as deleted
    }
};

REGISTER(BM_Unschedule_Cold, SlotMapScheduler);
REGISTER(BM_Schedule_Cold, SlotMapScheduler);
REGISTER(BM_Unschedule_Hot,  SlotMapScheduler);
REGISTER(BM_Schedule_Hot, SlotMapScheduler);

BENCHMARK_MAIN();