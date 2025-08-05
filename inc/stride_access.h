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
    void* memory_buffer_;
    size_t memory_size_;
    int iteration_;
    int access_stride_;
    bool initialized_;
};

#endif // STRIDE_ACCESS_H 