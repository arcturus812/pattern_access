#include "pointer_chase_access.h"
#include <iostream>
#include <cstring>
#include <algorithm>
#include <random>

bool PointerChaseAccess::init(void* memory_buffer, size_t memory_size, const json& config) {
    // TODO: Initialize pointer chase access pattern
    memory_buffer_ = memory_buffer;
    memory_size_ = memory_size;
    iteration_ = config.value("iteration", 1);
    verbose_ = config.value("verbose", false);
    initialized_ = true;

    // Build pointer chase chain
    build_pointer_chain();

    if (verbose_) {
        std::cout << "PointerChaseAccess initialized with: " << std::endl;
        std::cout << "  memory_size: " << memory_size_ << std::endl;
        std::cout << "  iteration: " << iteration_ << std::endl;
        std::cout << "  pointer_chain length: " << memory_size_ / sizeof(DataNode) << std::endl;
    }

    return true;
}

void PointerChaseAccess::access() {
    volatile uint64_t dummy_variable = 0;  // volatile to prevent optimization
    if (!initialized_) {
        std::cerr << "PointerChaseAccess not initialized" << std::endl;
        return;
    }

    for (int it = 0; it < iteration_; it++) {
        // Traverse the pointer chain
        traverse_pointer_chain();
    }
    
    // Prevent compiler from optimizing away the dummy variable
    (void)dummy_variable;

    if (verbose_) {
        std::cout << "Pointer chase access pattern executed" << std::endl;
    }
}

void PointerChaseAccess::build_pointer_chain() {
    size_t num_nodes = memory_size_ / sizeof(DataNode);
    
    if (num_nodes < 2) {
        std::cerr << "Error: Memory too small for pointer chase chain" << std::endl;
        return;
    }
    if (verbose_) {
        std::cout << "Building pointer chain with " << num_nodes << " nodes" << std::endl;
    }

    DataNode* nodes = static_cast<DataNode*>(memory_buffer_);
    
    // Initialize all nodes with zero addresses
    std::fill_n(nodes, num_nodes, DataNode{0});
    
    // Create and shuffle indices
    std::vector<size_t> indices(num_nodes);
    std::iota(indices.begin(), indices.end(), 0);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    if (verbose_) {
        std::cout << "Shuffle start" << std::endl;
    }
    std::shuffle(indices.begin(), indices.end(), gen);
    
    // Build circular linked list using addresses
    head_ = &nodes[indices[0]];
    
    for (size_t i = 0; i < num_nodes - 1; ++i) {
        size_t current_idx = indices[i];
        size_t next_idx = indices[i + 1];
        nodes[current_idx].next_address = 
            reinterpret_cast<uint64_t>(&nodes[next_idx]);
    }
    
    // Make it circular
    size_t last_idx = indices[num_nodes - 1];
    nodes[last_idx].next_address = reinterpret_cast<uint64_t>(head_);
    
    if (verbose_) {
        std::cout << "Built shuffled pointer chain with " << num_nodes << " nodes" << std::endl;
        std::cout << "Each node contains address to next node" << std::endl;
    }
}

void PointerChaseAccess::traverse_pointer_chain() {
    if (!head_) {
        std::cerr << "Error: Pointer chain not built" << std::endl;
        return;
    }

    volatile uint64_t dummy_variable = 0;
    DataNode* current = head_;
    size_t node_count = 0;
    size_t max_nodes = memory_size_ / sizeof(DataNode);
    
    for (int it = 0; it < iteration_; it++) {
        // Traverse the entire shuffled chain
        do {
            // Access the current node's address (force memory read)
            dummy_variable = current->next_address;
            
            // Move to next node using address
            current = reinterpret_cast<DataNode*>(current->next_address);
            node_count++;
            
            // Safety check to prevent infinite loops
            if (node_count > max_nodes) {
                std::cerr << "Warning: Potential infinite loop detected" << std::endl;
                break;
            }
        } while (current != head_);

        // Prevent compiler from optimizing away the dummy variable
        (void)dummy_variable;
    }
    
    
    if (verbose_) {
        std::cout << "Traversed " << node_count << " nodes in shuffled pointer chain" << std::endl;
    }
}
