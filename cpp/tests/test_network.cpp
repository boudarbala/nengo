#include <nengo/nengo.hpp>
#include <iostream>
#include <cassert>

// Basic assertion helper
#define ASSERT_TRUE(condition) \
    if (!(condition)) { \
        std::cerr << "ASSERTION FAILED: " << #condition << " at line " << __LINE__ << std::endl; \
        return 1; \
    }

#define ASSERT_EQ(a, b) \
    if ((a) != (b)) { \
        std::cerr << "ASSERTION FAILED: " << #a << " != " << #b << " (" << (a) << " != " << (b) << ") at line " << __LINE__ << std::endl; \
        return 1; \
    }

int test_network_creation() {
    std::cout << "Testing network creation..." << std::endl;
    
    auto network = std::make_shared<nengo::Network>("Test Network");
    ASSERT_EQ(network->getLabel(), "Test Network");
    ASSERT_EQ(network->getEnsembles().size(), 0);
    ASSERT_EQ(network->getNodes().size(), 0);
    ASSERT_EQ(network->getConnections().size(), 0);
    ASSERT_EQ(network->getProbes().size(), 0);
    
    std::cout << "Network creation test passed!" << std::endl;
    return 0;
}

int test_network_add_objects() {
    std::cout << "Testing network object addition..." << std::endl;
    
    auto network = std::make_shared<nengo::Network>();
    
    // Add ensemble
    auto ensemble = std::make_shared<nengo::Ensemble>(10, 1);
    network->add(ensemble);
    ASSERT_EQ(network->getEnsembles().size(), 1);
    ASSERT_TRUE(network->getEnsembles()[0] == ensemble);
    
    // Add node
    auto node = std::make_shared<nengo::Node>(1);
    network->add(node);
    ASSERT_EQ(network->getNodes().size(), 1);
    ASSERT_TRUE(network->getNodes()[0] == node);
    
    // Add connection
    auto connection = std::make_shared<nengo::Connection>(node, ensemble);
    network->add(connection);
    ASSERT_EQ(network->getConnections().size(), 1);
    ASSERT_TRUE(network->getConnections()[0] == connection);
    
    // Add probe
    auto probe = std::make_shared<nengo::Probe>(ensemble);
    network->add(probe);
    ASSERT_EQ(network->getProbes().size(), 1);
    ASSERT_TRUE(network->getProbes()[0] == probe);
    
    // Test getAllObjects
    auto all_objects = network->getAllObjects();
    ASSERT_EQ(all_objects.size(), 4);
    
    std::cout << "Network object addition test passed!" << std::endl;
    return 0;
}

int test_simulator_creation() {
    std::cout << "Testing simulator creation..." << std::endl;
    
    auto network = std::make_shared<nengo::Network>();
    auto ensemble = std::make_shared<nengo::Ensemble>(5, 1);
    network->add(ensemble);
    
    nengo::Simulator sim(network, 0.002);
    ASSERT_EQ(sim.getDt(), 0.002);
    ASSERT_EQ(sim.getTime(), 0.0);
    ASSERT_EQ(sim.getStep(), 0);
    ASSERT_TRUE(sim.getNetwork() == network);
    
    std::cout << "Simulator creation test passed!" << std::endl;
    return 0;
}

int test_simulation_step() {
    std::cout << "Testing simulation stepping..." << std::endl;
    
    auto network = std::make_shared<nengo::Network>();
    auto ensemble = std::make_shared<nengo::Ensemble>(5, 1);
    network->add(ensemble);
    
    nengo::Simulator sim(network, 0.001);
    
    // Test single step
    sim.step();
    ASSERT_EQ(sim.getStep(), 1);
    ASSERT_EQ(sim.getTime(), 0.001);
    
    // Test multiple steps
    sim.runSteps(5);
    ASSERT_EQ(sim.getStep(), 6);
    ASSERT_EQ(sim.getTime(), 0.006);
    
    // Test time-based run
    double initial_time = sim.getTime();
    sim.run(0.01);  // Run for 10ms
    ASSERT_TRUE(sim.getTime() >= initial_time + 0.01);
    
    std::cout << "Simulation stepping test passed!" << std::endl;
    return 0;
}

int test_simple_simulation() {
    std::cout << "Testing simple simulation..." << std::endl;
    
    auto network = std::make_shared<nengo::Network>();
    
    // Create constant input
    Eigen::VectorXd input_value(1);
    input_value << 0.5;
    auto input_node = std::make_shared<nengo::Node>(input_value);
    
    // Create ensemble
    auto ensemble = std::make_shared<nengo::Ensemble>(10, 1);
    
    // Connect them
    auto connection = std::make_shared<nengo::Connection>(input_node, ensemble);
    
    // Add probe
    auto probe = std::make_shared<nengo::Probe>(ensemble);
    
    // Add to network
    network->add(input_node);
    network->add(ensemble);
    network->add(connection);
    network->add(probe);
    
    // Run simulation
    nengo::Simulator sim(network, 0.001);
    sim.run(0.1);  // 100ms
    
    // Check that data was recorded
    auto data = sim.getData(probe);
    ASSERT_TRUE(data.size() > 0);
    
    std::cout << "Simple simulation test passed!" << std::endl;
    return 0;
}

int main() {
    std::cout << "Running network and simulation tests..." << std::endl;
    
    int result = 0;
    result |= test_network_creation();
    result |= test_network_add_objects();
    result |= test_simulator_creation();
    result |= test_simulation_step();
    result |= test_simple_simulation();
    
    if (result == 0) {
        std::cout << "All network tests passed!" << std::endl;
    } else {
        std::cout << "Some network tests failed!" << std::endl;
    }
    
    return result;
}