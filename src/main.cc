#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <CLI/CLI.hpp>
#include <fstream>
#include <chrono>

#include "common.h"
#include "access_wrapper.h"

using json = nlohmann::json;

int main(int argc, char *argv[]) {
    CLI::App app{"Pattern Access Tool"};
    
    // Define options
    std::string config_input;
    bool verbose = false;
    bool pause = false;
    
    // Add options to the app
    app.add_option("-c,--config", config_input, "config file path")
        ->required();
    
    app.add_flag("-v,--verbose", verbose, "Enable verbose output");

    app.add_flag("-p, --pause", pause, "Pause before access");
    
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

    // Alloc and init memory according to config's mem_size, numa_node
    std::string mem_size_str = config.value("mem_size", std::string("1G"));
    int numa_node = config.value("numa_node", 0);
    
    size_t mem_size_byte = parse_size_to_bytes(mem_size_str);
    
    // Round up memory size to multiple of 64 bytes (cache line size)
    const size_t ACCESS_UNIT_SIZE = 64;
    size_t aligned_size = ((mem_size_byte + ACCESS_UNIT_SIZE - 1) / ACCESS_UNIT_SIZE) * ACCESS_UNIT_SIZE;
    
    void* memory_buffer = allocate_numa_memory(aligned_size, numa_node);
    if (!memory_buffer) {
        std::cerr << "Failed to allocate memory" << std::endl;
        return 1;
    }
    
    if (verbose) {
        std::cout << "Memory allocated successfully:" << std::endl;
        std::cout << "  Size: " << mem_size_str << " (" << aligned_size << " bytes)" << std::endl;
        std::cout << "  NUMA node: " << numa_node << std::endl;
    }


    // Create and initialize AccessWrapper with selected pattern and config
    AccessWrapper access_wrapper;
    std::string pattern = config.value("access_pattern", std::string("sequential"));
    // add verbose to config element
    config["verbose"] = verbose;
    if (!access_wrapper.init(pattern, memory_buffer, aligned_size, config)) {
        std::cerr << "Failed to initialize AccessWrapper" << std::endl;
        deallocate_numa_memory(memory_buffer, aligned_size);
        return 1;
    }
    
    if (pause) {
        std::cout << "initialized, press Enter to access memory by \033[31m" << pattern << "\033[0m pattern" << std::endl;
        std::cin.get();
    }

    // Get the start timestamp (in milliseconds since epoch)
    auto start_timestamp = std::chrono::steady_clock::now();

    // run access wrapper
    access_wrapper.access();

    // Get the end timestamp (in milliseconds since epoch)
    auto end_timestamp = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_timestamp - start_timestamp);
    std::cout << "Time taken: " << duration.count() << " milliseconds" << std::endl; // TODO : print result
    

    // Clean up memory before exit
    deallocate_numa_memory(memory_buffer, aligned_size);

    return 0;
}