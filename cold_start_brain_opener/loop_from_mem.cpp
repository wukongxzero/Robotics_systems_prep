#include <chrono>
#include <thread>
#include <iostream>

int main() {
    using clock = std::chrono::steady_clock;

    const double dt = 0.01; // 100 Hz
    auto next = clock::now();

    double t = 0.0;

    while (t < 1.0) { // run for 1s
        auto start = clock::now();

        // ---- "work" section (replace with real logic) ----
        std::cout << "t=" << t << "\n";
        // -----------------------------------------------

        next += std::chrono::duration_cast<clock::duration>(
            std::chrono::duration<double>(dt)
        );
        std::this_thread::sleep_until(next);

        auto end = clock::now();
        std::chrono::duration<double> loop_time = end - start;

        t += dt;
    }
}
