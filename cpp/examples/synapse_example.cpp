#include <iostream>
#include <iomanip>
#include <nengo/nengo.hpp>

/**
 * Example demonstrating synapse usage in Nengo C++
 * 
 * This example shows how to:
 * 1. Create different types of synapses (Lowpass, Alpha, Triangle)
 * 2. Filter signals using synapses
 * 3. Compare their responses to a step input
 */

int main() {
    std::cout << "Nengo C++ Synapse Example" << std::endl;
    std::cout << "=========================" << std::endl;
    
    // Create different synapse types
    auto lowpass = std::make_shared<nengo::Lowpass>(0.1);      // tau = 0.1
    auto alpha = std::make_shared<nengo::Alpha>(0.05);         // tau = 0.05
    auto triangle = std::make_shared<nengo::Triangle>(0.05);   // t = 0.05
    
    std::cout << "\nCreated synapses:" << std::endl;
    std::cout << "- " << lowpass->toString() << std::endl;
    std::cout << "- " << alpha->toString() << std::endl;
    std::cout << "- " << triangle->toString() << std::endl;
    
    // Create a step input signal
    int n_steps = 50;
    double dt = 0.01;
    Eigen::VectorXd step_input(n_steps);
    
    // Step at time 0.1 (index 10)
    for (int i = 0; i < n_steps; ++i) {
        step_input(i) = (i >= 10) ? 1.0 : 0.0;
    }
    
    // Filter the step input with each synapse
    std::cout << "\nFiltering step input (step at t=0.1):" << std::endl;
    
    Eigen::VectorXd lowpass_output = lowpass->filt(step_input, dt);
    Eigen::VectorXd alpha_output = alpha->filt(step_input, dt);
    Eigen::VectorXd triangle_output = triangle->filt(step_input, dt);
    
    // Display some sample points
    std::cout << "\nTime\tInput\tLowpass\tAlpha\tTriangle" << std::endl;
    std::cout << "----\t-----\t-------\t-----\t--------" << std::endl;
    
    for (int i = 0; i < n_steps; i += 5) {
        double t = i * dt;
        std::cout << std::fixed << std::setprecision(2) 
                  << t << "\t" 
                  << step_input(i) << "\t"
                  << lowpass_output(i) << "\t"
                  << alpha_output(i) << "\t"
                  << triangle_output(i) << std::endl;
    }
    
    // Demonstrate synapse properties
    std::cout << "\nSynapse Properties:" << std::endl;
    std::cout << "- Lowpass tau: " << lowpass->getTau() << " seconds" << std::endl;
    std::cout << "- Alpha tau: " << alpha->getTau() << " seconds" << std::endl;
    std::cout << "- Triangle t: " << triangle->getT() << " seconds" << std::endl;
    
    // Show final steady-state values
    std::cout << "\nFinal values (should approach 1.0):" << std::endl;
    std::cout << "- Lowpass: " << lowpass_output(n_steps-1) << std::endl;
    std::cout << "- Alpha: " << alpha_output(n_steps-1) << std::endl;
    std::cout << "- Triangle: " << triangle_output(n_steps-1) << std::endl;
    
    // Example of using synapses polymorphically
    std::cout << "\nPolymorphic usage:" << std::endl;
    std::vector<std::shared_ptr<nengo::Synapse>> synapses = {lowpass, alpha, triangle};
    
    for (size_t i = 0; i < synapses.size(); ++i) {
        auto& syn = synapses[i];
        std::cout << "Synapse " << i << ": " << syn->toString() 
                  << " (dt=" << syn->getDefaultDt() << ")" << std::endl;
    }
    
    std::cout << "\nExample completed successfully!" << std::endl;
    return 0;
}