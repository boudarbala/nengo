# Nengo C++

A C++ translation of the [Nengo](https://www.nengo.ai/) neural simulation library, implementing the Neural Engineering Framework (NEF).

## Overview

Nengo C++ provides a C++ API for building and simulating large-scale neural models. It translates the core concepts from the Python Nengo library into modern C++, utilizing Eigen for linear algebra operations.

## Features

- **Core NEF Components**: Ensembles, Nodes, Connections, and Networks
- **Multiple Neuron Types**: Direct, LIF (Leaky Integrate-and-Fire), and LIF Rate neurons
- **Flexible Simulation**: Time-based and step-based simulation control
- **Data Collection**: Probes for recording simulation data
- **Modern C++**: Uses C++17 features and smart pointers for memory management

## Dependencies

- **CMake** (>= 3.10)
- **Eigen3** - Linear algebra library
- **C++17** compatible compiler

## Building

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build the library and examples
make

# Run tests (optional)
make test
```

## Quick Start

```cpp
#include <nengo/nengo.hpp>

int main() {
    // Create a network
    auto network = std::make_shared<nengo::Network>("My Network");
    
    // Create an ensemble of 100 neurons representing a 2D vector
    auto ensemble = std::make_shared<nengo::Ensemble>(100, 2);
    network->add(ensemble);
    
    // Create a probe to record data
    auto probe = std::make_shared<nengo::Probe>(ensemble);
    network->add(probe);
    
    // Run simulation
    nengo::Simulator sim(network, 0.001);  // 1ms timestep
    sim.run(1.0);  // Run for 1 second
    
    // Get recorded data
    auto data = sim.getDataMatrix(probe);
    std::cout << "Recorded " << data.rows() << " timesteps" << std::endl;
    
    return 0;
}
```

## Core Classes

### Network
Container for neural network components. Can contain ensembles, nodes, connections, probes, and other networks.

### Ensemble
Group of neurons that collectively represent a vector. Key parameters:
- `n_neurons`: Number of neurons
- `dimensions`: Representational dimensions
- `radius`: Range of represented values
- `neuron_type`: Type of neuron model

### Node
Non-neural computation element. Can provide input, process output, or implement arbitrary functions.

### Connection
Link between objects with optional transforms and functions. Supports:
- Linear transformations (matrices)
- Nonlinear functions
- Synaptic filtering (basic version)

### Probe
Data collection point for recording simulation values over time.

### Simulator
Executes the simulation, advancing the network state over time.

## Neuron Types

### Direct
Ideal neurons that directly represent values without neural dynamics.

### LIF (Leaky Integrate-and-Fire)
Spiking neuron model with realistic dynamics including refractory periods.

### LIFRate
Rate-based approximation of LIF neurons for faster simulation.

## Examples

The `examples/` directory contains:

- **simple_example.cpp**: Basic network creation and simulation
- **communication_channel.cpp**: Input-to-ensemble communication
- **ensemble_basics.cpp**: Comparison of different neuron types

## Differences from Python Nengo

This C++ version implements core NEF functionality but with some simplifications:

- Basic neuron models only (no adaptive types)
- Simplified learning rules (not implemented yet)
- Basic synaptic filtering
- Limited built-in functions and transforms

## Architecture

The library follows object-oriented design principles:

- **RAII**: Automatic resource management
- **Smart Pointers**: Shared ownership with automatic cleanup
- **Eigen Integration**: Efficient linear algebra operations
- **Type Safety**: Compile-time error checking

## Performance

C++ implementation provides:

- **Speed**: Compiled code with optimizations
- **Memory Efficiency**: Direct memory management
- **Parallelization**: Potential for multi-threading (future enhancement)

## Contributing

This is a basic translation focusing on core functionality. Potential enhancements:

- Advanced neuron types (Adaptive LIF, Izhikevich)
- Learning rules (PES, BCM, Oja)
- Convolution transforms
- GPU acceleration
- Parallel simulation
- Advanced synaptic models

## License

This C++ translation maintains compatibility with Nengo's licensing approach.

## References

- [Nengo Python Library](https://www.nengo.ai/)
- [Neural Engineering Framework](https://en.wikipedia.org/wiki/Neural_Engineering_Framework)
- [Eigen Documentation](https://eigen.tuxfamily.org/)