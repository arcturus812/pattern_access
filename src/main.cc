#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <CLI/CLI.hpp>
#include <fstream>
#include <memory>
#include <chrono>
#include <unistd.h>
#include "mem_access.h"
#include "common.h"

using json = nlohmann::json;

int main(int argc, char *argv[]) {
    CLI::App app{"Pattern Access Tool"};
    
    // Define options
    std::string config_input;
    bool verbose = false;
    
    // Add options to the app
    app.add_option("-c,--config", config_input, "config file path")
        ->required();
    
    app.add_flag("-v,--verbose", verbose, "Enable verbose output");
    
    // Parse command line arguments
    CLI11_PARSE(app, argc, argv);
    
    // Read and parse config file
    std::ifstream config_file(config_input);
    if (!config_file.is_open()) {
        std::cerr << "Error: Cannot open config file: " << config_input << std::endl;
        return 1;
    }
    
    json config;
    config_file >> config;
    
    if (verbose) {
        std::cout << "Loaded config from: " << config_input << std::endl;
        std::cout << "Config: " << config.dump(2) << std::endl;
    }
    
    // Create memory access object based on pattern
    std::unique_ptr<MemAccess> mem_accessor;
    
    std::string access_pattern = config.value("access_pattern", std::string("sequential"));
    if (access_pattern == "sequential") {
        mem_accessor.reset(new SequentialAccess());
        if (verbose) {
            std::cout << "Configured sequential access pattern" << std::endl;
        }
    } else {
        std::cerr << "Invalid access pattern: " << access_pattern << std::endl;
        return 1;
    }
    
    // Initialize memory access with config
    std::string mem_size_str = config.value("mem_size", std::string("1G"));
    size_t mem_size_byte = parse_size_to_bytes(mem_size_str);
    
    if (mem_accessor && mem_accessor->init(mem_size_byte, AccessPattern::SEQUENTIAL, config)) {
        if (verbose) {
            std::cout << "Memory access initialized successfully(size: " << mem_size_str << "(" << mem_size_byte << " bytes))" << std::endl;
        }
        std::cout << "PID: " << getpid() << std::endl;

        if (config.value("hesitate", false)) {
            // pause by using input
            std::cout << "Press Enter to continue..." << std::endl;
            std::cin.get();
        }
        
        // Measure access time
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Perform memory access iterations
        mem_accessor->access();
        
        auto end_time = std::chrono::high_resolution_clock::now();
        
        // Calculate elapsed time in microseconds
        auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        
        // Output timing results
        std::cout << "Memory access completed:" << std::endl;
        std::cout << "  Total time: " << elapsed_time.count() << " microseconds" << std::endl;
        
    } else {
        std::cerr << "Failed to initialize memory access" << std::endl;
        return 1;
    }
    
    return 0;
}