#ifndef POINTER_CHASE_ACCESS_H
#define POINTER_CHASE_ACCESS_H

#include "memory_access_base.h"

// Pointer chase access pattern implementation
class PointerChaseAccess : public MemoryAccessBase {
public:
    // Initialize pointer chase access pattern
    bool init(void* memory_buffer, size_t memory_size, const json& config) override;
    
    // Execute pointer chase access pattern
    void access() override;
    
    // Get pattern name
    const char* get_pattern_name() const override { return "pointer_chase"; }

private:
    void* memory_buffer_;  // Keep as void* for flexibility
    size_t memory_size_;
    int iteration_;
    bool verbose_;
    bool initialized_;
    
    // Pointer chase specific members
    struct DataNode {
        uint64_t next_address;
    };
    DataNode* head_;  // Head of the linked list
    
    // Helper method for accessing memory with proper alignment
    template<typename T>
    T* get_ptr_at_offset(size_t offset) const {
        return reinterpret_cast<T*>(static_cast<char*>(memory_buffer_) + offset);
    }
    
    // Helper method to build pointer chase chain
    void build_pointer_chain();
    
    // Helper method to traverse pointer chain
    void traverse_pointer_chain();
};

#endif // POIN