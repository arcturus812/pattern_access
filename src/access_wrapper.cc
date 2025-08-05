#include "access_wrapper.h"
#include "sequential_access.h"
#include "stride_access.h"
#include "dynamic_random_access.h"
#include <iostream>
#include <memory>

AccessWrapper::AccessWrapper() {
    // Constructor
}

AccessWrapper::~AccessWrapper() {
    // Destructor
}

std::unique_ptr<MemoryAccessBase> AccessWrapper::create_access_pattern(const std::string& pattern) {
    if (pattern == "sequential") {
        return std::unique_ptr<MemoryAccessBase>(new SequentialAccess());
    } else if (pattern == "stride") {
        return std::unique_ptr<MemoryAccessBase>(new StrideAccess());
    } else if (pattern == "dynamic_random") {
        return std::unique_ptr<MemoryAccessBase>(new DynamicRandomAccess());
    } else {
        std::cerr << "Unknown access pattern: " << pattern << std::endl;
        return nullptr;
    }
}

bool AccessWrapper::init(const std::string& pattern, void* memory_buffer, size_t memory_size, const json& config) {
    // Create access pattern instance
    access_impl_ = create_access_pattern(pattern);
    if (!access_impl_) {
        return false;
    }
    
    // Initialize the pattern
    return access_impl_->init(memory_buffer, memory_size, config);
}

void AccessWrapper::access() {
    if (access_impl_) {
        access_impl_->access();
    } else {
        std::cerr << "AccessWrapper not initialized" << std::endl;
    }
}

const char* AccessWrapper::get_current_pattern() const {
    return access_impl_ ? access_impl_->get_pattern_name() : "none";
} 