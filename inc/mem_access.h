#ifndef MEM_ACCESS_H
#define MEM_ACCESS_H

#include <cstddef>
#include <memory>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Memory access pattern enumeration
enum class AccessPattern {
    SEQUENTIAL = 0,
    // Future patterns can be added here
    RANDOM = 1,
    STRIDE = 2,
    PT_CHASE = 3,
    LINEAR = 4,
    UNKNOWN = 99
};

// Memory access unit size (64 bytes - typical cache line size)
static const size_t ACCESS_UNIT_SIZE = 64;
static const size_t PAGE_SIZE = 4096;

class MemAccess {
public:
    // Constructor with no required arguments
    MemAccess();
    
    // Virtual destructor for proper inheritance
    virtual ~MemAccess();
    
    // Initialize memory access with size, pattern, and config
    bool init(size_t memory_size, AccessPattern pattern, const json& config);
    
    // Virtual access function to be overridden
    virtual void access() = 0;
    
    // Check if initialization is complete
    bool is_initialized() const;
    

protected:
    // Memory buffer (NUMA-aware allocation)
    void* memory_buffer;
    
    // Memory size in bytes
    size_t mem_size;

    // NUMA node
    int numa_node;

    // index tables
    bool intra;
    std::vector<size_t> random_index_table;
    
    // Number of 64-byte access units
    size_t access_units;
    
    // Access pattern
    AccessPattern pattern;
    
    // Configuration values
    int iteration;
    int access_stride;
    int access_per_page;
    bool hesitate;
    
    // Initialization status
    bool initialized;

private:
    // NUMA-aware memory allocation
    bool allocate_numa_memory(size_t size, int node);
    
    // NUMA-aware memory deallocation
    void deallocate_numa_memory();
};

// Sequential access implementation
class SequentialAccess : public MemAccess {
public:
    SequentialAccess();
    virtual ~SequentialAccess();
    
    // Override access function for sequential pattern
    virtual void access() override;
};

#endif // MEM_ACCESS_H

// Random access implementation
class RandomAccess : public MemAccess {
public:
    RandomAccess();
    virtual ~RandomAccess();
    
    // Override access function for random pattern
    virtual void access() override;
};