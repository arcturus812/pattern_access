#ifndef STRIDE_ACCESS_H
#define STRIDE_ACCESS_H

#include "memory_access_base.h"

// Stride access pattern implementation
class StrideAccess : public MemoryAccessBase {
public:
    // Initialize stride access pattern
    bool init(void* memory_buffer, size_t memory_size, const json& config) override;
    
    // Execute stride access pattern
    void access() override;
    
    // Get pattern name
    const char* get_pattern_name() const override { return "stride"; }

private:
    void* memory_buffer_;  // Keep as void* for flexibility
    size_t memory_size_;
    int iteration_;
    int access_stride_;
    int byte_per_access_;
    bool verbose_;
    bool initialized_;
    
    // Helper method for accessing memory with proper alignment
    template<typename T>
    T* get_ptr_at_offset(size_t offset) const {
        return reinterpret_cast<T*>(static_cast<char*>(memory_buffer_) + offset);
    }
};

#endif // STRIDE_ACCESS_H 