#include <nengo/nengo.hpp>
#include <iostream>
#include <iomanip>
#include <cmath>

/**
 * Communication Channel Example
 * Demonstrates how to create a simple communication channel between
 * an input node and an ensemble.
 */
int main() {
    std::cout << "Nengo C++ Communication Channel Example" << std::endl;
    
    // Create a network
    auto network = std::make_shared<nengo::Network>("Communication Channel");
    
    // Create input node that generates a sine wave
    auto input_node = std::make_shared<nengo::Node>(
        [](double t, const Eigen::VectorXd& x) -> Eigen::VectorXd {
            Eigen::VectorXd output(1);
            output << std::sin(2 * M_PI * t);  // 1 Hz sine wave
            return output;
        },
        0,  // size_in
        1,  // size_out
        "Sine Input"
    );
    
    // Create an ensemble to receive and represent the input
    auto ensemble = std::make_shared<nengo::Ensemble>(
        50,                                     // n_neurons
        1,                                      // dimensions  
        1.0,                                    // radius
        std::make_shared<nengo::LIFRate>(),    // neuron type
        "Representation"
    );
    
    // Connect input to ensemble
    auto connection = std::make_shared<nengo::Connection>(
        input_node,
        ensemble,
        nullptr,                               // no function
        Eigen::MatrixXd::Identity(1, 1),      // identity transform
        "Input->Ensemble"
    );
    
    // Add components to network
    network->add(input_node);
    network->add(ensemble);
    network->add(connection);
    
    // Create probes
    auto input_probe = std::make_shared<nengo::Probe>(input_node, "", 0.001, "Input Probe");
    auto ensemble_probe = std::make_shared<nengo::Probe>(ensemble, "", 0.001, "Ensemble Probe");
    
    network->add(input_probe);
    network->add(ensemble_probe);
    
    // Run simulation
    nengo::Simulator sim(network, 0.001);
    
    std::cout << "Running communication channel simulation for 2.0 seconds..." << std::endl;
    sim.run(2.0);
    
    // Analyze results
    auto input_data = sim.getDataMatrix(input_probe);
    auto ensemble_data = sim.getDataMatrix(ensemble_probe);
    
    std::cout << "Input data shape: " << input_data.rows() << " x " << input_data.cols() << std::endl;
    std::cout << "Ensemble data shape: " << ensemble_data.rows() << " x " << ensemble_data.cols() << std::endl;
    
    // Print sample of data to show communication
    std::cout << "\nSample communication results:" << std::endl;
    std::cout << "Time\tInput\tEnsemble" << std::endl;
    for (int i = 0; i < std::min(10, static_cast<int>(input_data.rows())); i += 100) {
        double t = i * 0.001;
        std::cout << std::fixed << std::setprecision(3) 
                  << t << "\t" << input_data(i, 0) << "\t" << ensemble_data(i, 0) << std::endl;
    }
    
    std::cout << "\nCommunication channel simulation completed!" << std::endl;
    return 0;
}