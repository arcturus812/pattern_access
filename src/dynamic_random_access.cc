#include "dynamic_random_access.h"
#include <iostream>

bool DynamicRandomAccess::init(void* memory_buffer, size_t memory_size, const json& config) {
    // TODO: Initialize dynamic random access pattern
    memory_buffer_ = memory_buffer;
    memory_size_ = memory_size;
    iteration_ = config.value("iteration", 1);
    byte_per_access_ = config.value("byte_per_access", 64);
    verbose_ = config.value("verbose", false);
    random_state_ = config.value("random_seed", 12345);  // Default seed
    initialized_ = true;

    // Validate byte_per_access is power of 2
    if (byte_per_access_ <= 0 || (byte_per_access_ & (byte_per_access_ - 1)) != 0) {
        std::cerr << "Error: byte_per_access must be a power of 2, got: " << byte_per_access_ << std::endl;
        return false;
    }

    if (verbose_) {
        std::cout << "DynamicRandomAccess initialized with: " << std::endl;
        std::cout << "  memory_size: " << memory_size_ << std::endl;
        std::cout << "  iteration: " << iteration_ << std::endl;
        std::cout << "  byte_per_access: " << byte_per_access_ << std::endl;
        std::cout << "  random_seed: " << random_state_ << std::endl;
    }

    return true;
}

void DynamicRandomAccess::access() {
    volatile uint64_t dummy_variable = 0;  // volatile to prevent optimization
    if (!initialized_) {
        std::cerr << "DynamicRandomAccess not initialized" << std::endl;
        return;
    }

    for (int it = 0; it < iteration_; it++) {
        // Access memory with dynamic random pattern
        size_t num_elements = memory_size_ / byte_per_access_;
        for (size_t i = 0; i < num_elements; i++) {
            // Generate random offset within memory bounds
            uint64_t random_offset = fast_rand_64bit(&random_state_) % num_elements;
            size_t byte_offset = random_offset * byte_per_access_;
            
            // Ensure offset is within memory bounds
            if (byte_offset < memory_size_) {
                dummy_variable = *get_ptr_at_offset<uint64_t>(byte_offset);
            }
        }
    }
    
    // Prevent compiler from optimizing away the dummy variable
    (void)dummy_variable;

    if (verbose_) {
        std::cout << "Dynamic random access pattern executed" << std::endl;
    }
}

uint64_t DynamicRandomAccess::fast_rand_64bit(uint64_t *state) {
    // wyhash64
    // refer to https://lemire.me/blog/2019/03/19/the-fastest-conventional-random-number-generator-that-can-pass-big-crush/
    __uint128_t tmp;
    uint64_t m1, m2;
    *state += 0x60bee2bee120fc15;
    tmp = (__uint128_t) (*state) * 0xa3b195354a39b70d;
    m1 = (tmp >> 64) ^ tmp;
    tmp = (__uint128_t)m1 * 0x1b03738712fad5c9;
    m2 = (tmp >> 64) ^ tmp;
    return m2;
}
