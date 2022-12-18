#pragma once
#include <cinttypes>

struct BufferSpan {
    using T = const uint8_t;
    T* data{};
    size_t count{};

    auto begin() const -> T* { return data; }
    auto end() const -> T* { return data + count; }
    auto operator[](size_t idx) const -> T { return data[idx]; }

    auto seek(size_t offset) const -> BufferSpan {
        if (offset >= count) return {};
        return {data + offset, count - offset};
    }
    auto truncate_to(size_t to) const -> BufferSpan {
        if (count < to) return {};
        return {data, to};
    }
    auto is_content_equal(BufferSpan other) const -> bool {
        if (count != other.count) return false;
        for (auto i = 0u; i < count; i++)
            if (data[i] != other[i]) return false;
        return true;
    }
    template<class T> auto le_cast() -> T {
        static constexpr auto size = sizeof(T);
        auto result = T{};
        if (count < size) return result;
        if constexpr (size == 1) {
            result = data[0];
        }
        else if constexpr (size == 2) {
            result = static_cast<T>((static_cast<uint16_t>(data[1]) << 8u) + data[0]);
        }
        else if constexpr (size == 4) {
            result = static_cast<T>(
                (static_cast<uint32_t>(data[3]) << 24u) + (static_cast<uint32_t>(data[2]) << 16u) +
                (static_cast<uint32_t>(data[1]) << 8u) + data[0]);
        }
        else {
            static_assert(size != -1, "unhandled");
        }
        return result;
    }
    void each_string(auto callback) const {
        auto b = 0u;
        while (true) {
            while (b < count && data[b] == 0) b++;
            auto e = b + 1;
            while (e < count && data[e] != 0) e++;
            if (b < count)
                callback(seek(b).truncate_to(e - b));
            else
                break;
            b = e + 1;
        }
    }
};
