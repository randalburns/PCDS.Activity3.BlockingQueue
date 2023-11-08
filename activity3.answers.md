solutions to the coding portion of the vectorization homework
The easiest, just convert to parallel using divide and conquer

```
uint64_t serial(uint8_t *data, uint64_t size, uint8_t target) {
  uint64_t total = 0;
  for (uint64_t i = 0; i < size - 1; i++) {
    if (data[i] == target && data[i + 1] == target) {
      total += 1;
    }
  }
  return total;
}

uint64_t count_pairs(uint8_t *data, uint64_t n, uint8_t target) {
  if (n < (1 << 15)) {
    return serial(data, n, target);
  }
  long x, y;
  cilk_scope {
    x = cilk_spawn count_pairs(data, n / 2, target);
    y = count_pairs(data + (n / 2), n - (n / 2), target);
  }
  return x + y;
}
```
if you just add a cilk_for and a reducer it won't vectorize

```
__attribute__((weak)) uint64_t
solution_simple_parallel(uint8_t *data, long size, uint8_t target) {
  cilk::opadd_reducer<uint64_t> total = 0;
  uint16_t check = target | (target << 8U);
  cilk_for(uint64_t i = 0; i < size - 1; i++) {
    total += (load16(data + i) == check);
  }
  return total;
}
```

another non intrisic based one, with some casting and loop splitting the compiler does a pretty good job
```
uint64_t serial(uint8_t *data, uint64_t size, uint8_t target) {
  uint64_t total = 0;
  uint16_t target_short = (target) | (target << 8U);
  uint16_t *data_short = (uint16_t *) data;
  for (uint64_t i = 0; i < size/2; i++) {
    if (data_short[i] == target_short) {
      total += 1;
    }
  }
  data_short = (uint16_t *) (data + 1);
  for (uint64_t i = 0; i < size/2 -1; i++) {
    if (data_short[i] == target_short) {
      total += 1;
    }
  }
  return total;
}
```
intrinsic based solutions, very similar to what I showed in class with one extra bit trick
```
__attribute__((weak)) long serial(uint8_t *data, long size,
                                               uint8_t target) {
  uint64_t total = 0;
  uint32_t last_bit = 0;
  __m256i compare = _mm256_set1_epi8(target);
  int clean_end = (size / 32) * 32;
  uint64_t i = 0;
  for (; i < clean_end; i += 32) {
    uint32_t block = _mm256_movemask_epi8(
        _mm256_cmpeq_epi8(_mm256_loadu_si256((__m256i *)(data + i)), compare));
    total += __builtin_popcount(block & (block >> 1U));
    if (last_bit) {
      total += last_bit & block;
    }
    last_bit = block >> 31U;
  }
  if (i > 0) {
    i -= 1;
  }
  uint16_t check = target | (target << 8U);
  for (; i < size - 1; i++) {
    total += (load16(data + i) == check);
  }
  return total;
}
```

intrinsic based solutions, very similar to what I showed in class with one extra bit trick
```
__attribute__((weak)) long serial(uint8_t *data, long size,
                                               uint8_t target) {
  uint64_t total = 0;
  uint32_t last_bit = 0;
  __m256i compare = _mm256_set1_epi8(target);
  int clean_end = (size / 32) * 32;
  uint64_t i = 0;
  for (; i < clean_end; i += 32) {
    uint32_t block = _mm256_movemask_epi8(
        _mm256_cmpeq_epi8(_mm256_loadu_si256((__m256i *)(data + i)), compare));
    total += __builtin_popcount(block & (block >> 1U));
    if (last_bit) {
      total += last_bit & block;
    }
    last_bit = block >> 31U;
  }
  if (i > 0) {
    i -= 1;
  }
  uint16_t check = target | (target << 8U);
  for (; i < size - 1; i++) {
    total += (load16(data + i) == check);
  }
  return total;
}

```

separate loads and doing the shift over 1 byte still in vector registers (basis for this one came from chatgpt)

```
uint64_t count_pairs(uint8_t *data, uint64_t size, uint8_t target) {
    cilk::opadd_reducer<uint64_t> total = 0;
    __m256i target_vector = _mm256_set1_epi8(target);
    __m256i* data_ptr = (__m256i*)data;
    cilk_for (uint64_t i = 0; i < (size - 1) / 32; i++) {
        __m256i data1 = _mm256_loadu_si256(data_ptr+i);
        __m256i data2 = _mm256_loadu_si256(data_ptr+i + 1);
        __m256i compare1 = _mm256_cmpeq_epi8(data1, target_vector);
        __m256i compare2 = _mm256_cmpeq_epi8(data2, target_vector);
        __m256i combined_compare = _mm256_and_si256(compare1, compare2);
        int mask = _mm256_movemask_epi8(combined_compare);
        total += _mm_popcnt_u32(mask);
    }
    return total;
}
also, we are not paying super attention to off by ones and the like, or reading extra data off the end (edited) 








