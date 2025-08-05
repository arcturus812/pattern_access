#include "stride_access.h"
#include <iostream>

bool StrideAccess::init(void* memory_buffer, size_t memory_size, const json& config) {
    // TODO: Initialize stride access pattern
    memory_buffer_ = memory_buffer;
    memory_size_ = memory_size;
    iteration_ = config.value("iteration", 1);
    access_stride_ = config.value("access_stride", 4);
    byte_per_access_ = config.value("byte_per_access", 64);
    verbose_ = config.value("verbose", false);
    initialized_ = true;

    // Validate byte_per_access is power of 2
    if (byte_per_access_ <= 0 || (byte_per_access_ & (byte_per_access_ - 1)) != 0) {
        std::cerr << "Error: byte_per_access must be a power of 2, got: " << byte_per_access_ << std::endl;
        return false;
    }

    if (verbose_) {
        std::cout << "StrideAccess initialized with: " << std::endl;
        std::cout << "  memory_size: " << memory_size_ << std::endl;
        std::cout << "  iteration: " << iteration_ << std::endl;
        std::cout << "  access_stride: " << access_stride_ << std::endl;
        std::cout << "  byte_per_access: " << byte_per_access_ << std::endl;
    }

    return true;
}

void StrideAccess::access() {
    volatile uint64_t dummy_variable = 0;  // volatile to prevent optimization
    if (!initialized_) {
        std::cerr << "StrideAccess not initialized" << std::endl;
        return;
    }

    for (int it = 0; it < iteration_; it++) {
        // Access memory with stride pattern
        size_t num_elements = memory_size_ / byte_per_access_;
        for (size_t i = 0; i < num_elements; i += access_stride_) {
            dummy_variable = *get_ptr_at_offset<uint64_t>(i * byte_per_access_);
        }
    }
    
    // Prevent compiler from optimizing away the dummy variable
    (void)dummy_variable;

    if (verbose_) {
        std::cout << "Stride access pattern executed" << std::endl;
    }
} 