
#pragma once

#include <benchmark/benchmark.h>

#include <cstddef>
#include <stdexcept>
#include <thread>

template <typename Queue>
inline static void BM(benchmark::State& state) {
    const auto n_iters = state.range(0);
    const auto capacity = static_cast<std::size_t>(state.range(1));

    for (auto _ : state) {
        auto queue = Queue{capacity};

        auto consumer = std::thread([&] {
            auto item = typename Queue::value_type{};
            for (auto i = 0; i < n_iters; ++i) {
                while (!queue.try_pop(item)) {
                }
                if (item != i) {
                    state.SkipWithError("Data check failed");
                }
            }
        });

        auto start = std::chrono::high_resolution_clock::now();
        for (auto i = 0; i < n_iters; ++i) {
            while (!queue.try_push(i)) {
            }
        }
        consumer.join();

        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed_seconds =
            std::chrono::duration_cast<std::chrono::duration<double>>(end -
                                                                      start);
        state.SetIterationTime(elapsed_seconds.count());
    }

    state.SetItemsProcessed(state.iterations() * n_iters);
}

#define SPSC_REGISTER_QUEUE_BENCHMARK(queue_type) \
    BENCHMARK(BM<queue_type<int>>)                \
        ->Args({100'000, 16})                     \
        ->Args({100'000, 1'024})                  \
        ->Args({100'000, 1'048'576})              \
        ->ArgNames({"items", "capacity"})         \
        ->UseManualTime()                         \
        ->Unit(benchmark::kMillisecond)
