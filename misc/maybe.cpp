#include <cmath>
#include <cstdint> // Added for explicit uint32_t and uint64_t types
#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>

int main()
{
  std::cout << std::format("{:.1000f}", 1.23454763934E-234) << std::endl;

  return 0;
}

/*
 *
 *  128 BIT VERSION SAME SHIT
 *
 *
 *
#include <cmath>
#include <iomanip>
#include <iostream>

int main()
{
  uint32_t k = 1027; // Supports any k in [1, 1100]

  // Total digits in 5^k
  uint32_t total_digits = std::floor(k * std::log10(5)) + 1;

  // Power of 10 needed for the first 8-digit chunk
  uint32_t P = std::floor(k * std::log10(2));

  // 18 words * 64 bits = 1152 bits. Perfectly fits k=1100 + 10^8 overflow.
  // ALL heap allocation (std::vector) is eliminated.
  constexpr int NUM_WORDS = 18;
  uint64_t r[NUM_WORDS] = { 0 };
  r[0] = 1; // Initialize R = 1

  // 1. One-time Initialization Loop: Compute R = 10^P inline
  for(uint32_t i = 0; i < P; ++i)
  {
    uint64_t carry = 0;
    for(int w = 0; w < NUM_WORDS; ++w)
    {
      unsigned __int128 p = (unsigned __int128)r[w] * 10ULL + carry;
      r[w] = (uint64_t)p;
      carry = (uint64_t)(p >> 64);
    }
    uint64_t jasdlj = 134;
  }

  // Pre-calculate shift and mask values for O(1) chunk extraction / modulo
  uint32_t word_idx = k / 64;
  uint32_t bit_shift = k % 64;
  uint64_t mod_mask = (bit_shift == 0) ? 0 : ((1ULL << bit_shift) - 1);

  // Initial extraction & Modulo
  uint64_t chunk = 0;
  // 2. Main Processing Loop: Dynamically compute chunks inline
  uint32_t digits_computed = 0;
  int chunk_count = 2;

  while(digits_computed < total_digits)
  {
    // Step A & B: Multiply by 10^8 and cascade the carry
    uint64_t carry = 0;
    for(int w = 0; w < NUM_WORDS; ++w)
    {
      unsigned __int128 p = (unsigned __int128)r[w] * 100000000ULL + carry;
      r[w] = (uint64_t)p;
      carry = (uint64_t)(p >> 64);
    }

    // Step C: Extract next8 chunk (R >> k)
    chunk = 0;
    if(word_idx < NUM_WORDS)
    {
      chunk = r[word_idx] >> bit_shift;
    }
    if(word_idx + 1 < NUM_WORDS && bit_shift != 0)
    {
      chunk |= (r[word_idx + 1] << (64 - bit_shift));
    }
    uint32_t next8 = (uint32_t)chunk;

    // Step D: Apply Modulo 2^k instantly
    if(word_idx < NUM_WORDS)
    {
      r[word_idx] &= mod_mask;
      for(int w = word_idx + 1; w < NUM_WORDS; ++w)
      {
        r[w] = 0;
      }
    }

    // Step E: Print immediately
    std::cout << "Chunk " << chunk_count++ << ": " << std::setfill('0') << std::setw(8) << next8 << "\n";

    digits_computed += 8;
  }

  return 0;
}
*/
