#include <nengo/nengo.hpp>
#include <iostream>

/**
 * Simple Nengo C++ example
 * Creates a basic network with an ensemble and runs a simulation.
 */
int main() {
    std::cout << "Nengo C++ Simple Example" << std::endl;
    std::cout << "Version: " << nengo::VERSION << std::endl;
    
    // Create a network
    auto network = std::make_shared<nengo::Network>("Simple Network");
    
    // Create an ensemble of 100 LIF neurons representing a 2D vector
    auto ensemble = std::make_shared<nengo::Ensemble>(
        100,                                    // n_neurons
        2,                                      // dimensions
        1.0,                                    // radius
        std::make_shared<nengo::LIFRate>(),    // neuron type
        "Population"                            // label
    );
    
    // Add the ensemble to the network
    network->add(ensemble);
    
    // Create a probe to record ensemble output
    auto probe = std::make_shared<nengo::Probe>(ensemble, "", 0.001, "Ensemble Probe");
    network->add(probe);
    
    // Create and run simulator
    nengo::Simulator sim(network, 0.001);  // 1ms timestep
    
    std::cout << "Running simulation for 1.0 seconds..." << std::endl;
    sim.run(1.0);
    
    // Get recorded data
    auto data = sim.getDataMatrix(probe);
    std::cout << "Recorded " << data.rows() << " timesteps of " 
              << data.cols() << "D data" << std::endl;
    
    // Print some sample data points
    std::cout << "First 5 timesteps:" << std::endl;
    for (int i = 0; i < std::min(5, static_cast<int>(data.rows())); ++i) {
        std::cout << "t=" << i * 0.001 << "s: [";
        for (int j = 0; j < data.cols(); ++j) {
            std::cout << data(i, j);
            if (j < data.cols() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }
    
    std::cout << "Simulation completed successfully!" << std::endl;
    return 0;
}