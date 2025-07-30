#!/bin/bash

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== Pattern Access Project Preparation Script ===${NC}"

# Function to print status
print_status() {
    echo -e "${YELLOW}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if vcpkg directory exists, if not clone and bootstrap
if [ ! -d "vcpkg" ]; then
    print_status "vcpkg directory not found. Installing vcpkg..."
    
    # Clone vcpkg repository
    print_status "Cloning vcpkg from GitHub..."
    git clone https://github.com/Microsoft/vcpkg.git
    
    if [ $? -eq 0 ]; then
        print_success "vcpkg cloned successfully"
    else
        print_error "Failed to clone vcpkg"
        exit 1
    fi
    
    # Bootstrap vcpkg
    print_status "Bootstrapping vcpkg..."
    cd vcpkg
    ./bootstrap-vcpkg.sh
    
    if [ $? -eq 0 ]; then
        print_success "vcpkg bootstrapped successfully"
    else
        print_error "Failed to bootstrap vcpkg"
        exit 1
    fi
    
    cd ..
else
    # 1. vcpkg update and install required libraries
    print_status "vcpkg found. Updating vcpkg..."
    cd vcpkg

    # Update vcpkg to latest version
    git pull
    ./bootstrap-vcpkg.sh

    print_success "vcpkg updated successfully"

    # Go back to project root
    cd ..
fi

# Install dependencies using vcpkg
print_status "Installing dependencies via vcpkg..."
./vcpkg/vcpkg install --triplet=x64-linux

if [ $? -eq 0 ]; then
    print_success "Dependencies installed successfully"
else
    print_error "Failed to install dependencies"
    exit 1
fi

# 2. Create build directory and run cmake
print_status "Setting up build directory..."

# Remove existing build directory if it exists
if [ -d "build" ]; then
    print_status "Removing existing build directory..."
    rm -rf build
fi

# Create new build directory
mkdir build
cd build

print_status "Running CMake configuration..."
cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake

if [ $? -eq 0 ]; then
    print_success "CMake configuration completed successfully"
else
    print_error "CMake configuration failed"
    exit 1
fi

# 3. Run make
print_status "Building project..."
make

if [ $? -eq 0 ]; then
    print_success "Build completed successfully"
    print_status "Executable created: bin/micro-access"
else
    print_error "Build failed"
    exit 1
fi

# Go back to project root
cd ..

print_success "Project preparation completed successfully!"
echo -e "${GREEN}=== Ready to run! ===${NC}"
echo "Usage: ./build/bin/micro-access -c configs/config.json -v"
