#ifndef ACCESS_WRAPPER_H
#define ACCESS_WRAPPER_H

#include <memory>
#include <string>
#include <nlohmann/json.hpp>
#include "memory_access_base.h"

using json = nlohmann::json;

// Simple AccessWrapper class
class AccessWrapper {
public:
    AccessWrapper();
    ~AccessWrapper();
    
    // Initialize with pattern and config
    bool init(const std::string& pattern, void* memory_buffer, size_t memory_size, const json& config);
    
    // Execute access pattern
    void access();
    
    // Get current pattern name
    const char* get_current_pattern() const;

private:
    std::unique_ptr<MemoryAccessBase> access_impl_;
    
    // Factory method to create access pattern instances
    std::unique_ptr<MemoryAccessBase> create_access_pattern(const std::string& pattern);
};

#endif // ACCESS_WRAPPER_H