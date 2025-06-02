#pragma once
#include <span>
#include <cstring>
#include <cassert>

namespace lblf {

template<typename T>
void read_blf_struct(const std::span<const std::byte> data, T& out) {
    assert(data.size() >= sizeof(T));
    std::memcpy(&out, data.data(), sizeof(T));
}

} // namespace lblf
