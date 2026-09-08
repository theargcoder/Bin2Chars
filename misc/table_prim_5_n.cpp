#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>

int main()
{
  uint32_t k = 1027;

  // Total digits in 5^128
  uint32_t total_digits = std::floor(k * std::log10(5)) + 1; // 90 digits

  uint64_t r0 = 7392204872816828115U;
  uint64_t r1 = 10379699663911648153U;

  uint32_t digits_computed = 0;
  int chunk_count = 2;

  while(digits_computed < total_digits)
  {
    // Step A: Multiply the lower 64 bits by 10^8
    unsigned __int128 p0 = (unsigned __int128)r0 * 100000000ULL;
    r0 = (uint64_t)p0;

    // Step B: Multiply the upper 64 bits and add the overflow/carry from p0
    unsigned __int128 p1 = (unsigned __int128)r1 * 100000000ULL + (p0 >> 64);
    r1 = (uint64_t)p1;

    // Step C: Extract next8 chunk & Apply Modulo 2^128 instantly
    // The bits overflowing past bit 128 represent (R >> 128), captured perfectly by p1 >> 64
    uint32_t next8 = (uint32_t)(p1 >> 64);

    // Step D: Print immediately
    std::cout << "Chunk " << chunk_count++ << ": " << std::setfill('0') << std::setw(8) << next8 << "\n";

    digits_computed += 8;
  }

  return 0;
}
