#include <nengo/nengo.hpp>
#include <iostream>

/**
 * Ensemble Basics Example
 * Demonstrates different neuron types and ensemble properties.
 */
int main() {
    std::cout << "Nengo C++ Ensemble Basics Example" << std::endl;
    
    // Create different neuron types
    auto lif_neurons = std::make_shared<nengo::LIF>(0.002, 0.02, 1.0);
    auto lif_rate_neurons = std::make_shared<nengo::LIFRate>(0.002, 0.02, 1.0);
    auto direct_neurons = std::make_shared<nengo::Direct>();
    
    std::cout << "\nCreated neuron types:" << std::endl;
    std::cout << "- LIF: tau_ref=" << lif_neurons->getTauRef() 
              << ", tau_rc=" << lif_neurons->getTauRc() << std::endl;
    std::cout << "- LIFRate: similar parameters but rate-based" << std::endl;
    std::cout << "- Direct: ideal neurons (no dynamics)" << std::endl;
    
    // Create ensembles with different neuron types
    auto network = std::make_shared<nengo::Network>("Ensemble Comparison");
    
    auto ens_lif = std::make_shared<nengo::Ensemble>(
        100, 2, 1.0, lif_neurons, "LIF Ensemble");
    auto ens_lifrate = std::make_shared<nengo::Ensemble>(
        100, 2, 1.0, lif_rate_neurons, "LIFRate Ensemble");
    auto ens_direct = std::make_shared<nengo::Ensemble>(
        2, 2, 1.0, direct_neurons, "Direct Ensemble");  // Only 2 neurons needed for direct
    
    // Add to network
    network->add(ens_lif);
    network->add(ens_lifrate);
    network->add(ens_direct);
    
    // Create constant input
    Eigen::VectorXd input_value(2);
    input_value << 0.5, -0.3;
    auto input_node = std::make_shared<nengo::Node>(input_value, 0, "Constant Input");
    network->add(input_node);
    
    // Connect input to all ensembles
    auto conn_lif = std::make_shared<nengo::Connection>(input_node, ens_lif);
    auto conn_lifrate = std::make_shared<nengo::Connection>(input_node, ens_lifrate);
    auto conn_direct = std::make_shared<nengo::Connection>(input_node, ens_direct);
    
    network->add(conn_lif);
    network->add(conn_lifrate);
    network->add(conn_direct);
    
    // Create probes
    auto probe_input = std::make_shared<nengo::Probe>(input_node);
    auto probe_lif = std::make_shared<nengo::Probe>(ens_lif);
    auto probe_lifrate = std::make_shared<nengo::Probe>(ens_lifrate);
    auto probe_direct = std::make_shared<nengo::Probe>(ens_direct);
    
    network->add(probe_input);
    network->add(probe_lif);
    network->add(probe_lifrate);
    network->add(probe_direct);
    
    // Print ensemble properties
    std::cout << "\nEnsemble properties:" << std::endl;
    std::cout << "LIF Ensemble: " << ens_lif->getNeurons() << " neurons, " 
              << ens_lif->getDimensions() << "D, radius=" << ens_lif->getRadius() << std::endl;
    std::cout << "LIFRate Ensemble: " << ens_lifrate->getNeurons() << " neurons, "
              << ens_lifrate->getDimensions() << "D, radius=" << ens_lifrate->getRadius() << std::endl;
    std::cout << "Direct Ensemble: " << ens_direct->getNeurons() << " neurons, "
              << ens_direct->getDimensions() << "D, radius=" << ens_direct->getRadius() << std::endl;
    
    // Run simulation
    nengo::Simulator sim(network, 0.001);
    
    std::cout << "\nRunning simulation for 0.5 seconds..." << std::endl;
    sim.run(0.5);
    
    // Analyze results
    auto data_input = sim.getDataMatrix(probe_input);
    auto data_lif = sim.getDataMatrix(probe_lif);
    auto data_lifrate = sim.getDataMatrix(probe_lifrate);
    auto data_direct = sim.getDataMatrix(probe_direct);
    
    std::cout << "\nFinal values (should approximate input [0.5, -0.3]):" << std::endl;
    int last_idx = data_input.rows() - 1;
    
    std::cout << "Input:    [" << data_input(last_idx, 0) << ", " << data_input(last_idx, 1) << "]" << std::endl;
    std::cout << "LIF:      [" << data_lif(last_idx, 0) << ", " << data_lif(last_idx, 1) << "]" << std::endl;
    std::cout << "LIFRate:  [" << data_lifrate(last_idx, 0) << ", " << data_lifrate(last_idx, 1) << "]" << std::endl;
    std::cout << "Direct:   [" << data_direct(last_idx, 0) << ", " << data_direct(last_idx, 1) << "]" << std::endl;
    
    std::cout << "\nEnsemble basics example completed!" << std::endl;
    return 0;
}