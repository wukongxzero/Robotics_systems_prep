#include <array>
#include <cstddef>
#include <optional>

template <typename T, std::size_t N>
class RingBuffer {
public:
    bool push(const T& v) {
        if (count_ == N) return false; // full
        buf_[tail_] = v;
        tail_ = (tail_ + 1) % N;
        count_++;
        return true;
    }

    std::optional<T> pop() {
        if (count_ == 0) return std::nullopt; // empty
        T v = buf_[head_];
        head_ = (head_ + 1) % N;
        count_--;
        return v;
    }

    std::size_t size() const { return count_; }
    bool empty() const { return count_ == 0; }
    bool full() const { return count_ == N; }

private:
    std::array<T, N> buf_{};
    std::size_t head_ = 0;
    std::size_t tail_ = 0;
    std::size_t count_ = 0;
};
