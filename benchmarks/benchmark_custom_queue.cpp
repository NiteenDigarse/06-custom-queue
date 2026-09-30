#include "custom_queue.hpp"

#include <chrono>
#include <cstddef>
#include <iostream>

int main()
{
    constexpr std::size_t N = 1'000'000;

    custom::CustomQueue<int> queue(N);

    // -----------------------------
    // Enqueue Benchmark
    // -----------------------------

    auto start = std::chrono::steady_clock::now();

    for (std::size_t i = 0; i < N; ++i) {
        queue.enqueue(static_cast<int>(i));
    }

    auto end = std::chrono::steady_clock::now();

    auto enqueue_time =
        std::chrono::duration_cast<std::chrono::microseconds>(
            end - start
        );

    std::cout << "Enqueue " << N
              << " elements: "
              << enqueue_time.count()
              << " us\n";


    // -----------------------------
    // Dequeue Benchmark
    // -----------------------------

    start = std::chrono::steady_clock::now();

    while (!queue.empty()) {
        queue.dequeue();
    }

    end = std::chrono::steady_clock::now();

    auto dequeue_time =
        std::chrono::duration_cast<std::chrono::microseconds>(
            end - start
        );

    std::cout << "Dequeue " << N
              << " elements: "
              << dequeue_time.count()
              << " us\n";


    return 0;
}