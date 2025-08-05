#ifndef MEMORY_ACCESS_BASE_H
#define MEMORY_ACCESS_BASE_H

#include <cstddef>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Abstract base class for all access patterns
class MemoryAccessBase {
public:
    MemoryAccessBase() = default;
    virtual ~MemoryAccessBase() = default;
    
    // Pure virtual functions that must be implemented
    virtual bool init(void* memory_buffer, size_t memory_size, const json& config) = 0;
    virtual void access() = 0;
    virtual const char* get_pattern_name() const = 0;
};

#endif // MEMORY_ACCESS_BASE_H
