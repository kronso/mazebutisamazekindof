#pragma once
#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>

struct Timer {
    std::chrono::high_resolution_clock::time_point start, end;
    std::chrono::duration<float> duration;
    // when struct is instantiated, will call the default constructor
    Timer () {
        start = std::chrono::high_resolution_clock::now();
    }

    // when function is destroyed, will all the destructor
    ~Timer() {
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        float ms = duration.count() * 1000.0f;
        std::cout << std::fixed;
        std::cout << std::setprecision(2) << "Duration: " << ms << " ms\n\n"; // 2 d.p.
    }
};