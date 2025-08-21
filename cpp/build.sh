#!/bin/bash

# Build script for Nengo C++

set -e  # Exit on any error

echo "Building Nengo C++..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake ..

# Build
echo "Building library and examples..."
make -j$(nproc)

# Run tests
echo "Running tests..."
if make test; then
    echo "All tests passed!"
else
    echo "Some tests failed!"
    exit 1
fi

# Run examples
echo "Running examples..."
echo "=== Simple Example ==="
./examples/simple_example

echo ""
echo "=== Communication Channel Example ==="
./examples/communication_channel

echo ""
echo "=== Ensemble Basics Example ==="
./examples/ensemble_basics

echo ""
echo "Build and test completed successfully!"