#ifndef SEQUENTIAL_ACCESS_H
#define SEQUENTIAL_ACCESS_H

#include "memory_access_base.h"

// Sequential access pattern implementation
class SequentialAccess : public MemoryAccessBase {
public:
    // Initialize sequential access pattern
    bool init(void* memory_buffer, size_t memory_size, const json& config) override;
    
    // Execute sequential access pattern
    void access() override;
    
    // Get pattern name
    const char* get_pattern_name() const override { return "sequential"; }

private:
    void* memory_buffer_;  // Keep as void* for flexibility
    size_t memory_size_;
    int iteration_;
    int access_per_page_;
    int byte_per_access_;
    bool verbose_;
    bool initialized_;
    
    // Helper method for accessing memory with proper alignment
    template<typename T>
    T* get_ptr_at_offset(size_t offset) const {
        return reinterpret_cast<T*>(static_cast<char*>(memory_buffer_) + offset);
    }
};

#endif // SEQUENTIAL_ACCESS_H 