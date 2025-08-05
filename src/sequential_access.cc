#include "sequential_access.h"
#include <iostream>

bool SequentialAccess::init(void* memory_buffer, size_t memory_size, const json& config) {
    // TODO: Initialize sequential access pattern
    memory_buffer_ = memory_buffer;
    memory_size_ = memory_size;
    iteration_ = config.value("iteration", 0);
    access_per_page_ = config.value("access_per_page", 0); // temporary ignore
    byte_per_access_ = config.value("byte_per_access", 0);
    verbose_ = config.value("verbose", false);
    initialized_ = true;

    // Validate byte_per_access is power of 2
    if (byte_per_access_ <= 0 || (byte_per_access_ & (byte_per_access_ - 1)) != 0) {
        std::cerr << "Error: byte_per_access must be a power of 2, got: " << byte_per_access_ << std::endl;
        return false;
    }

    if (verbose_) {
        std::cout << "SequentialAccess initialized with: " << std::endl;
        std::cout << "  memory_size: " << memory_size_ << std::endl;
        std::cout << "  iteration: " << iteration_ << std::endl;
        std::cout << "  access_per_page will be ignored" << std::endl;
        std::cout << "  byte_per_access: " << byte_per_access_ << std::endl;
    }

    return true;
}

void SequentialAccess::access() {
    volatile uint64_t dummy_variable = 0;
    if (!initialized_) {
        std::cerr << "SequentialAccess not initialized" << std::endl;
        return;
    }

    // Access memory in byte_per_access units
    size_t num_elements = memory_size_ / byte_per_access_;
    for (size_t i = 0; i < num_elements; i++) {
        dummy_variable = *get_ptr_at_offset<uint64_t>(i * byte_per_access_);
        //auto* ptr = get_ptr_at_offset<uint64_t>(i * byte_per_access_);
        //dummy_variable = *ptr;
    }
    (void)dummy_variable;

    if (verbose_) {
        std::cout << "Sequential access pattern executed" << std::endl;
    }
} 