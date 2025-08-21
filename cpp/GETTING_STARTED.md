# Nengo C++ Translation - Getting Started

This document provides a quick guide to using the newly translated C++ version of Nengo.

## Installation and Building

1. **Prerequisites:**
   ```bash
   sudo apt install libeigen3-dev cmake build-essential
   ```

2. **Build the library:**
   ```bash
   cd cpp
   mkdir build && cd build
   cmake ..
   make -j$(nproc)
   ```

3. **Run tests:**
   ```bash
   ./tests/test_basic
   ./tests/test_network
   ```

4. **Run examples:**
   ```bash
   ./examples/simple_example
   ./examples/communication_channel
   ./examples/ensemble_basics
   ```

## Quick API Comparison

### Python Nengo vs C++ Nengo

**Python:**
```python
import nengo

with nengo.Network() as net:
    ens = nengo.Ensemble(100, 2)
    input_node = nengo.Node([0.5, -0.3])
    nengo.Connection(input_node, ens)
    probe = nengo.Probe(ens)

with nengo.Simulator(net) as sim:
    sim.run(1.0)
    data = sim.data[probe]
```

**C++:**
```cpp
#include <nengo/nengo.hpp>

auto net = std::make_shared<nengo::Network>();
auto ens = std::make_shared<nengo::Ensemble>(100, 2);

Eigen::VectorXd input_val(2);
input_val << 0.5, -0.3;
auto input_node = std::make_shared<nengo::Node>(input_val);

auto conn = std::make_shared<nengo::Connection>(input_node, ens);
auto probe = std::make_shared<nengo::Probe>(ens);

net->add(ens);
net->add(input_node);
net->add(conn);
net->add(probe);

nengo::Simulator sim(net);
sim.run(1.0);
auto data = sim.getDataMatrix(probe);
```

## Key Differences

1. **Memory Management:** C++ uses smart pointers (`std::shared_ptr`) instead of Python's automatic garbage collection.

2. **Linear Algebra:** Uses Eigen instead of NumPy for matrix operations.

3. **Explicit Object Management:** Must explicitly add objects to networks.

4. **Type Safety:** Compile-time type checking catches errors early.

## Neuron Types Available

- `nengo::Direct` - Ideal neurons (no dynamics)
- `nengo::LIF` - Leaky Integrate-and-Fire neurons
- `nengo::LIFRate` - Rate-based LIF approximation

## Core Classes

- `nengo::Network` - Container for network components
- `nengo::Ensemble` - Groups of neurons representing vectors
- `nengo::Node` - Non-neural computation elements
- `nengo::Connection` - Links between objects
- `nengo::Probe` - Data recording
- `nengo::Simulator` - Simulation execution

## Performance Benefits

The C++ version offers:
- **Faster execution** due to compiled code
- **Lower memory usage** with explicit memory management
- **Better optimization** opportunities for the compiler
- **Integration** with existing C++ projects

## Limitations vs Python Version

Current C++ implementation is a foundational translation focusing on core NEF functionality:

- Limited to basic neuron types
- No learning rules yet
- Basic synaptic filtering only
- Simplified parameter distributions

## Future Enhancements

Potential areas for expansion:
- Advanced neuron models (Adaptive LIF, Izhikevich)
- Learning rules (PES, BCM, Oja)
- Convolutional transforms
- GPU acceleration with CUDA
- Parallel simulation
- Integration with neural frameworks

## Integration Example

Using Nengo C++ in a larger application:

```cpp
#include <nengo/nengo.hpp>
#include <iostream>

class NeuralController {
private:
    std::shared_ptr<nengo::Network> network_;
    std::unique_ptr<nengo::Simulator> sim_;
    
public:
    NeuralController() {
        // Build network
        network_ = std::make_shared<nengo::Network>("Controller");
        auto control_ens = std::make_shared<nengo::Ensemble>(50, 2);
        // ... add components
        
        sim_ = std::make_unique<nengo::Simulator>(network_, 0.001);
    }
    
    Eigen::VectorXd step(const Eigen::VectorXd& input) {
        // Process one timestep and return output
        sim_->step();
        // ... extract output
        return output;
    }
};
```

This C++ translation provides a solid foundation for high-performance neural modeling while maintaining the conceptual clarity of the Neural Engineering Framework.