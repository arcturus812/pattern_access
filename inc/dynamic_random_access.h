#ifndef DYNAMIC_RANDOM_ACCESS_H
#define DYNAMIC_RANDOM_ACCESS_H

#include "memory_access_base.h"

// Dynamic random access pattern implementation
class DynamicRandomAccess : public MemoryAccessBase {
public:
    // Initialize dynamic random access pattern
    bool init(void* memory_buffer, size_t memory_size, const json& config) override;
    
    // Execute dynamic random access pattern
    void access() override;
    
    // Get pattern name
    const char* get_pattern_name() const override { return "dynamic_random"; }

private:
    void* memory_buffer_;  // Keep as void* for flexibility
    size_t memory_size_;
    int iteration_;
    int byte_per_access_;
    bool verbose_;
    bool initialized_;
    uint64_t random_state_;  // State for random number generator
    
    // Helper method for accessing memory with proper alignment
    template<typename T>
    T* get_ptr_at_offset(size_t offset) const {
        return reinterpret_cast<T*>(static_cast<char*>(memory_buffer_) + offset);
    }
    
    // Fast random number generator (wyhash64)
    uint64_t fast_rand_64bit(uint64_t *state);
};

#endif // DYNAMIC_RANDOM_ACCESS_H
