#include <nengo/nengo.hpp>
#include <iostream>

int main() {
    std::cout << "Nengo C++ Networks Example" << std::endl;
    std::cout << "============================" << std::endl;

    // Create a main network
    auto network = std::make_shared<nengo::Network>("Networks Demo");

    std::cout << "\n1. Creating EnsembleArray..." << std::endl;
    // Create an ensemble array with 3 ensembles, each with 50 neurons and 2 dimensions
    auto ensemble_array = std::make_shared<nengo::networks::EnsembleArray>(
        50,  // neurons per ensemble
        3,   // number of ensembles
        2,   // dimensions per ensemble
        "demo_ea"
    );
    network->add(ensemble_array);
    
    std::cout << "   " << ensemble_array->toString() << std::endl;
    std::cout << "   Total dimensions: " << ensemble_array->getDimensions() << std::endl;
    std::cout << "   Sub-ensembles: " << ensemble_array->getEaEnsembles().size() << std::endl;

    std::cout << "\n2. Creating Product network..." << std::endl;
    // Create a product network for 2-dimensional multiplication
    auto product = std::make_shared<nengo::networks::Product>(
        100, // neurons per ensemble
        2,   // dimensions
        1.0, // input magnitude
        "demo_product"
    );
    network->add(product);
    
    std::cout << "   " << product->toString() << std::endl;

    std::cout << "\n3. Creating connections..." << std::endl;
    // Create some input nodes
    auto input_a = std::make_shared<nengo::Node>(2, "input_a");
    auto input_b = std::make_shared<nengo::Node>(2, "input_b");
    network->add(input_a);
    network->add(input_b);

    // Connect inputs to the product network
    auto conn_a = std::make_shared<nengo::Connection>(input_a, product->getInputA());
    auto conn_b = std::make_shared<nengo::Connection>(input_b, product->getInputB());
    network->add(conn_a);
    network->add(conn_b);

    // Connect the product output to the ensemble array
    auto conn_prod_ea = std::make_shared<nengo::Connection>(
        product->getOutput(), 
        ensemble_array->getInput()
    );
    network->add(conn_prod_ea);

    std::cout << "\n4. Creating probes for data collection..." << std::endl;
    // Create probes to monitor the network
    auto probe_input_a = std::make_shared<nengo::Probe>(input_a);
    auto probe_input_b = std::make_shared<nengo::Probe>(input_b);
    auto probe_product = std::make_shared<nengo::Probe>(product->getOutput());
    auto probe_ea_output = std::make_shared<nengo::Probe>(ensemble_array->getOutput());
    
    network->add(probe_input_a);
    network->add(probe_input_b);
    network->add(probe_product);
    network->add(probe_ea_output);

    std::cout << "\n5. Network summary..." << std::endl;
    std::cout << "   Network: " << network->toString() << std::endl;
    std::cout << "   Objects in network: " << network->getAllObjects().size() << std::endl;

    std::cout << "\n6. Creating simulator..." << std::endl;
    nengo::Simulator sim(network);
    std::cout << "   Simulator created successfully!" << std::endl;

    std::cout << "\nNetwork construction complete!" << std::endl;
    std::cout << "This demonstrates a neural network that:" << std::endl;
    std::cout << "- Takes two 2D inputs (input_a and input_b)" << std::endl;
    std::cout << "- Computes their element-wise product using the Product network" << std::endl;
    std::cout << "- Feeds the result into an EnsembleArray for further processing" << std::endl;
    std::cout << "\nIn a full implementation, you could run the simulation with:" << std::endl;
    std::cout << "   sim.run(1.0);  // Run for 1 second" << std::endl;

    return 0;
}