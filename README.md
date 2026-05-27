# Robotics Systems Prep

Real-time C++ patterns and algorithms for robotics technical interviews — written cold from memory, then verified. Covers the data structures and concurrency primitives that come up in embedded/robotics SWE roles.

## Contents

### `cold_start_brain_opener/`

Patterns written from scratch without references — the real drill:

| File | What it implements |
|------|--------------------|
| `ring_buffer_DS.cpp` | Lock-free ring buffer — fixed-size circular queue with `push`/`pop`, `std::optional` on empty, template over `T` and capacity `N` |
| `loop_from_mem.cpp` | Event loop and timer patterns from memory |

### `Systems_algos/`

| File | What it covers |
|------|---------------|
| `control_and_timing.cpp` | Periodic task timing, deadline management, WCET patterns |
| `Thread_safety_and_queue.cpp` | Thread-safe queue with mutex + condition variable, producer/consumer pattern |

## Ring buffer pattern

```cpp
template <typename T, std::size_t N>
class RingBuffer {
    std::array<T, N> buf_;
    std::size_t head_ = 0, tail_ = 0, count_ = 0;
public:
    bool push(const T& v) {
        if (count_ == N) return false;     // full — caller decides what to do
        buf_[tail_] = v;
        tail_ = (tail_ + 1) % N;
        count_++;
        return true;
    }
    std::optional<T> pop() {
        if (count_ == 0) return std::nullopt;
        T v = buf_[head_];
        head_ = (head_ + 1) % N;
        count_--;
        return v;
    }
};
```

Key properties: no heap allocation, O(1) push/pop, size fixed at compile time. Used in embedded contexts where `malloc` is forbidden on the hot path.

## Why this matters for robotics

Real-time robotics code avoids dynamic allocation on the control path. Ring buffers for sensor data, lock-free queues for inter-thread communication, and fixed-period timers for deterministic loop timing are the building blocks of a safe embedded/RT stack.

## Tech stack

C++17 · no external dependencies
