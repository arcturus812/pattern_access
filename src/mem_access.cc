#include "mem_access.h"
#include <stdexcept>
#include <new>
#include <cstring>
#include <iostream>

// Include NUMA headers only if available
#ifdef HAVE_NUMA
#include <numa.h>
#include <numaif.h>
#endif

// MemAccess base class implementation
MemAccess::MemAccess() : memory_buffer(nullptr), mem_size(0), numa_node(0),
                         access_units(0), pattern(AccessPattern::SEQUENTIAL), 
                         iteration(1), access_stride(4), access_per_page(8), 
                         hesitate(false), initialized(false) {
    // Default constructor with no arguments required
}

MemAccess::~MemAccess() {
    // Virtual destructor for proper cleanup
    deallocate_numa_memory();
}

bool MemAccess::allocate_numa_memory(size_t size, int node) {
#ifdef HAVE_NUMA
    // Check if NUMA is available
    if (numa_available() < 0) {
        std::cerr << "NUMA not available, using regular malloc" << std::endl;
        memory_buffer = malloc(size);
        if (memory_buffer) {
            memset(memory_buffer, 0, size);
            return true;
        }
        return false;
    }
    
    // Check if the requested NUMA node is valid
    int max_node = numa_max_node();
    if (node < 0 || node > max_node) {
        std::cerr << "Invalid NUMA node " << node << ", using node 0" << std::endl;
        node = 0;
    }
    
    // Allocate memory on specific NUMA node
    memory_buffer = numa_alloc_onnode(size, node);
    if (!memory_buffer) {
        std::cerr << "Failed to allocate memory on NUMA node " << node << std::endl;
        return false;
    }
    
    // Initialize memory
    memset(memory_buffer, 0, size);
    
    // Verify memory is allocated on correct NUMA node (optional)
    int allocated_node = -1;
    if (get_mempolicy(&allocated_node, nullptr, 0, memory_buffer, MPOL_F_NODE | MPOL_F_ADDR) == 0) {
        if (allocated_node != node) {
            std::cerr << "Warning: Memory allocated on node " << allocated_node 
                      << " instead of requested node " << node << std::endl;
        }
    }
    
    return true;
#else
    // NUMA not available, use regular malloc
    std::cerr << "NUMA support not compiled in, using regular malloc" << std::endl;
    memory_buffer = malloc(size);
    if (memory_buffer) {
        memset(memory_buffer, 0, size);
        return true;
    }
    return false;
#endif
}

void MemAccess::deallocate_numa_memory() {
    if (memory_buffer) {
#ifdef HAVE_NUMA
        if (numa_available() >= 0) {
            numa_free(memory_buffer, mem_size);
        } else {
            free(memory_buffer);
        }
#else
        free(memory_buffer);
#endif
        memory_buffer = nullptr;
    }
}

bool MemAccess::init(size_t memory_size, AccessPattern access_pattern, const json& config) {
    try {
        // Extract configuration values
        iteration = config.value("iteration", 1);
        access_stride = config.value("access_stride", 4);
        access_per_page = config.value("access_per_page", 8);
        hesitate = config.value("hesitate", false);
        numa_node = config.value("numa_node", 0);

        // Round up memory size to multiple of ACCESS_UNIT_SIZE
        size_t aligned_size = ((memory_size + ACCESS_UNIT_SIZE - 1) / ACCESS_UNIT_SIZE) * ACCESS_UNIT_SIZE;
        
        // Allocate NUMA-aware memory buffer
        if (!allocate_numa_memory(aligned_size, numa_node)) {
            return false;
        }
        
        mem_size = aligned_size;
        access_units = aligned_size / ACCESS_UNIT_SIZE;
        pattern = access_pattern;
        if(pattern == AccessPattern::RANDOM) {
            // TODO: Implement random index table
        }
        if(pattern == AccessPattern::PT_CHASE) {
            // TODO: Implement PT-chase index table
        }
        if(pattern == AccessPattern::LINEAR) {
            // TODO: Implement linear index table
        }
        initialized = true;
        
        return true;
    } catch (const std::exception& e) {
        // Config parsing or other errors
        initialized = false;
        return false;
    }
}

bool MemAccess::is_initialized() const {
    return initialized;
}

// SequentialAccess implementation
SequentialAccess::SequentialAccess() : MemAccess() {
    // Constructor inherits from base class
}

SequentialAccess::~SequentialAccess() {
    // Destructor
}

void SequentialAccess::access() {
    // Check if initialization is complete before access
    if (!is_initialized()) {
        throw std::runtime_error("MemAccess not initialized. Call init() first.");
    }

    // Calculate number of uint64_t elements per page (4KB / 8 bytes = 512)
    const size_t uint64_per_page = PAGE_SIZE / sizeof(uint64_t);
    // PAGE_SIZE is kb, mem_size is byte
    const size_t total_pages = mem_size / (PAGE_SIZE * 1024);
    volatile uint64_t dummy = 0; // Use volatile to prevent optimization

    uint64_t* ptr = static_cast<uint64_t*>(memory_buffer);

    for (int iter = 0; iter < iteration; iter++) {
        for (size_t i = 0; i < total_pages; i++) {
            for (size_t j = 0; j < static_cast<size_t>(access_per_page); j++) {
                dummy = ptr[i * uint64_per_page + j];
            }
        }
    }
    
    // Prevent compiler from optimizing away the dummy variable
    (void)dummy;
}
