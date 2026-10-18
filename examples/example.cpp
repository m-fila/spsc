#include <iostream>
#include "spsc/spsc.hpp"
#include <stdexcept>
#include <thread>

int main() {
    // Create a new queue with a fixed buffer of at least 4 elements
    spsc::queue<int> queue(4);

    // Producer
    std::thread producer([&queue]() {
        for (int i = 0; i < 10; ++i) {
            while (!queue.try_push(i)) {
                std::this_thread::yield();
            }
        }
    });

    // Consumer
    std::thread consumer([&queue]() {
        for (int i = 0; i < 10; ++i) {
            int value = -1;
            while (!queue.try_pop(value)) {
                std::this_thread::yield();
            }
            if (value != i) {
                throw std::runtime_error("Unexpected value popped from queue");
            } else {
                std::cout << "Consumed: " << value << std::endl;
            }
        }
    });

    producer.join();
    consumer.join();
    return 0;
}
