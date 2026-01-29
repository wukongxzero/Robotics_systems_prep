#include <condition_variable>
#include <chrono>
#include <iostream>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>

// Minimal thread-safe queue
template <typename T>
class TSQueue {
public:
    void push(T v) {
        {
            std::lock_guard<std::mutex> lk(m_);
            q_.push(std::move(v));
        }
        cv_.notify_one();
    }

    // Non-blocking try_pop
    std::optional<T> try_pop() {
        std::lock_guard<std::mutex> lk(m_);
        if (q_.empty()) return std::nullopt;
        T v = std::move(q_.front());
        q_.pop();
        return v;
    }

    // Blocking pop with timeout (useful for real systems)
    std::optional<T> pop_for(std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lk(m_);
        if (!cv_.wait_for(lk, timeout, [&]{ return !q_.empty(); })) {
            return std::nullopt;
        }
        T v = std::move(q_.front());
        q_.pop();
        return v;
    }

private:
    std::mutex m_;
    std::condition_variable cv_;
    std::queue<T> q_;
};

int main() {
    using clock = std::chrono::steady_clock;

    TSQueue<int> q;
    bool running = true;

    // Producer thread (e.g., sensor callback)
    std::thread producer([&]{
        for (int i = 0; i < 100; i++) {
            q.push(i);
            std::this_thread::sleep_for(std::chrono::milliseconds(15));
        }
        running = false;
    });

    // Consumer loop (e.g., control loop at 100 Hz)
    const double dt = 0.01; // 100 Hz
    auto next = clock::now();

    int latest = -1;
    while (running) {
        // Pull newest data (non-blocking), keep latest
        while (auto v = q.try_pop()) {
            latest = *v;
        }

        // "Control" uses latest measurement
        std::cout << "latest=" << latest << "\n";

        next += std::chrono::duration_cast<clock::duration>(
            std::chrono::duration<double>(dt)
        );
        std::this_thread::sleep_until(next);
    }

    producer.join();
}
