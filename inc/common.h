#include <iostream>
#include <string>
#include <unordered_map>
#include <cctype>
#include <stdexcept>
#include <sstream>
#include <cmath>
#include <cstdint>
#include <cstring>

// Include NUMA headers only if available
#ifdef HAVE_NUMA
#include <numa.h>
#include <numaif.h>
#endif

uint64_t parse_size_to_bytes(const std::string& input) {
    std::string trimmed;
    for (char c : input) {
        if (!std::isspace(static_cast<unsigned char>(c))) trimmed += c;
    }

    size_t idx = 0;
    while (idx < trimmed.size() && (std::isdigit(trimmed[idx]) || trimmed[idx] == '.')) {
        ++idx;
    }

    if (idx == 0) throw std::invalid_argument("Invalid input: " + input);

    double value = std::stod(trimmed.substr(0, idx));
    std::string unit = trimmed.substr(idx);

    static const std::unordered_map<std::string, uint64_t> unit_table = {
        {"b", 1ULL},
        {"k", 1ULL << 10}, {"kb", 1ULL << 10},
        {"m", 1ULL << 20}, {"mb", 1ULL << 20},
        {"g", 1ULL << 30}, {"gb", 1ULL << 30},
        {"t", 1ULL << 40}, {"tb", 1ULL << 40},
        {"kib", 1ULL << 10}, {"mib", 1ULL << 20},
        {"gib", 1ULL << 30}, {"tib", 1ULL << 40},
        {"B", 1ULL}, {"K", 1ULL << 10}, {"M", 1ULL << 20}, {"G", 1ULL << 30}, {"T", 1ULL << 40},
        {"KB", 1ULL << 10}, {"MB", 1ULL << 20}, {"GB", 1ULL << 30}, {"TB", 1ULL << 40},
        {"KiB", 1ULL << 10}, {"MiB", 1ULL << 20}, {"GiB", 1ULL << 30}, {"TiB", 1ULL << 40}
    };

    auto it = unit_table.find(unit);
    if (it == unit_table.end()) {
        throw std::invalid_argument("Unknown unit: " + unit);
    }

    return static_cast<uint64_t>(std::round(value * it->second));
}

// Memory allocation function
void* allocate_numa_memory(size_t size, int node) {
    void* memory_buffer = nullptr;
    
#ifdef HAVE_NUMA
    // Check if NUMA is available
    if (numa_available() < 0) {
        std::cerr << "NUMA not available, using regular malloc" << std::endl;
        memory_buffer = malloc(size);
        if (memory_buffer) {
            memset(memory_buffer, 0, size);
        }
        return memory_buffer;
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
        return nullptr;
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
    
#else
    // NUMA not available, use regular malloc
    std::cerr << "NUMA support not compiled in, using regular malloc" << std::endl;
    memory_buffer = malloc(size);
    if (memory_buffer) {
        memset(memory_buffer, 0, size);
    }
#endif
    
    return memory_buffer;
}

// Memory deallocation function
void deallocate_numa_memory(void* memory_buffer, size_t size) {
    if (memory_buffer) {
#ifdef HAVE_NUMA
        if (numa_available() >= 0) {
            numa_free(memory_buffer, size);
        } else {
            free(memory_buffer);
        }
#else
        free(memory_buffer);
#endif
    }
}