# Nengo C++ Architecture

```
Nengo C++ Library Structure
===========================

├── include/nengo/           # Header files
│   ├── nengo.hpp           # Main include (includes all others)
│   ├── base.hpp            # Base classes (NengoObject, Process)
│   ├── network.hpp         # Network container class
│   ├── ensemble.hpp        # Neuron ensemble class
│   ├── node.hpp            # Non-neural computation class
│   ├── connection.hpp      # Object connection class
│   ├── probe.hpp           # Data recording class
│   ├── simulator.hpp       # Simulation engine class
│   └── neurons.hpp         # Neuron type implementations
│
├── src/                    # Implementation files
│   ├── base.cpp
│   ├── network.cpp
│   ├── ensemble.cpp
│   ├── node.cpp
│   ├── connection.cpp
│   ├── probe.cpp
│   ├── simulator.cpp
│   └── neurons.cpp
│
├── examples/               # Example programs
│   ├── simple_example.cpp
│   ├── communication_channel.cpp
│   └── ensemble_basics.cpp
│
├── tests/                  # Test suite
│   ├── test_basic.cpp
│   └── test_network.cpp
│
└── docs/
    ├── README.md
    └── GETTING_STARTED.md

Class Hierarchy
===============

NengoObject (base)
├── Network
├── Ensemble
├── Node
├── Connection
└── Probe

NeuronType (base)
├── Direct
├── LIF
└── LIFRate

Process (base)
└── [Future: Various process types]

Key Relationships
=================

Network contains:
  - std::vector<Ensemble>
  - std::vector<Node>
  - std::vector<Connection>
  - std::vector<Probe>
  - std::vector<Network>  (nested networks)

Ensemble has:
  - NeuronType (polymorphic)
  - Encoders matrix (Eigen::MatrixXd)
  - Gain/Bias vectors
  - Neurons inner class

Connection links:
  - Pre object (Ensemble or Node)
  - Post object (Ensemble or Node)
  - Transform matrix
  - Function (optional)

Simulator manages:
  - Network reference
  - State vectors for all objects
  - Time stepping logic
  - Probe data collection

Data Flow
=========

1. Input → Node → Connection → Ensemble
2. Ensemble: input → encoders → neurons → output
3. Output → Connection → target object
4. Probe records values at each timestep

Memory Management
================

- Smart pointers (std::shared_ptr) for object ownership
- Eigen matrices for efficient linear algebra
- RAII for automatic resource cleanup
- No manual memory management required

Integration Points
==================

External System ←→ Node ←→ Network ←→ Ensemble
                     ↑         ↑         ↑
                  C++ API   Connections  Neural
                            Functions   Dynamics

Performance Characteristics
===========================

- Compile-time optimizations
- Efficient matrix operations (Eigen)
- Minimal runtime overhead
- Scalable to large networks
- Memory-efficient representations
```