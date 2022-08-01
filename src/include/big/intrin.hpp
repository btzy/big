#if defined(_MSC_VER)
#include <intrin.h>
#else
#include <immintrin.h>
#endif
#include <cstdint>

namespace big {
namespace detail {
namespace intrin {
// Add-with-carry variants.  8 and 16 bit versions are slow but it wouldn't appear in practice.
#if defined(_M_X64) || defined(__x86_64_)  // x64 MSVC or x64 GCC/Clang
unsigned char addcarry(unsigned char c_in, std::uint64_t a, std::uint64_t b, std::uint64_t& out) {
    return _addcarry_u64(c_in, a, b, &out);
}
#endif
unsigned char addcarry(unsigned char c_in, std::uint32_t a, std::uint32_t b, std::uint32_t& out) {
    return _addcarry_u32(c_in, a, b, &out);
}
unsigned char addcarry(unsigned char c_in, std::uint16_t a, std::uint16_t b, std::uint16_t& out) {
    std::uint32_t tmp;
    _addcarry_u32(c_in, a, b, &tmp);
    out = tmp;
    return static_cast<unsigned char>(tmp >> 16);
}
unsigned char addcarry(unsigned char c_in, std::uint8_t a, std::uint8_t b, std::uint8_t& out) {
    std::uint32_t tmp;
    _addcarry_u32(c_in, a, b, &tmp);
    out = tmp;
    return static_cast<unsigned char>(tmp >> 8);
}
}  // namespace intrin

}  // namespace detail

}  // namespace big
