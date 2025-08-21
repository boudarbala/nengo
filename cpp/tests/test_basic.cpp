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

int test_neuron_types() {
    std::cout << "Testing neuron types..." << std::endl;
    
    // Test Direct neurons
    auto direct = std::make_shared<nengo::Direct>();
    Eigen::VectorXd input(3);
    input << 1.0, -0.5, 2.0;
    Eigen::VectorXd output(3);
    
    direct->step(input, output);
    ASSERT_TRUE(output.isApprox(input));
    
    // Test LIF neurons
    auto lif = std::make_shared<nengo::LIF>();
    lif->step(input, output);
    ASSERT_EQ(output.size(), 3);
    
    std::cout << "Neuron types test passed!" << std::endl;
    return 0;
}

int test_ensemble() {
    std::cout << "Testing ensemble..." << std::endl;
    
    auto ensemble = std::make_shared<nengo::Ensemble>(10, 2, 1.5);
    
    ASSERT_EQ(ensemble->getNeurons(), 10);
    ASSERT_EQ(ensemble->getDimensions(), 2);
    ASSERT_EQ(ensemble->getRadius(), 1.5);
    ASSERT_TRUE(ensemble->getNeuronType() != nullptr);
    
    // Test encoders
    ASSERT_EQ(ensemble->getEncoders().rows(), 10);
    ASSERT_EQ(ensemble->getEncoders().cols(), 2);
    
    std::cout << "Ensemble test passed!" << std::endl;
    return 0;
}

int test_node() {
    std::cout << "Testing node..." << std::endl;
    
    // Test constant node
    Eigen::VectorXd constant_value(2);
    constant_value << 1.0, -0.5;
    auto node = std::make_shared<nengo::Node>(constant_value);
    
    ASSERT_EQ(node->getSizeOut(), 2);
    
    Eigen::VectorXd output = node->run(0.0);
    ASSERT_TRUE(output.isApprox(constant_value));
    
    // Test passthrough node
    auto passthrough = std::make_shared<nengo::Node>(3);
    ASSERT_EQ(passthrough->getSizeIn(), 3);
    ASSERT_EQ(passthrough->getSizeOut(), 3);
    
    Eigen::VectorXd input(3);
    input << 1.0, 2.0, 3.0;
    output = passthrough->run(0.0, input);
    ASSERT_TRUE(output.isApprox(input));
    
    std::cout << "Node test passed!" << std::endl;
    return 0;
}

int test_connection() {
    std::cout << "Testing connection..." << std::endl;
    
    auto pre_node = std::make_shared<nengo::Node>(2);
    auto post_node = std::make_shared<nengo::Node>(2);
    
    auto connection = std::make_shared<nengo::Connection>(pre_node, post_node);
    
    ASSERT_TRUE(connection->getPre() == pre_node);
    ASSERT_TRUE(connection->getPost() == post_node);
    
    // Test computation
    Eigen::VectorXd input(2);
    input << 1.0, -1.0;
    Eigen::VectorXd output = connection->compute(input);
    ASSERT_TRUE(output.isApprox(input));  // Should be identity transform
    
    std::cout << "Connection test passed!" << std::endl;
    return 0;
}

int test_probe() {
    std::cout << "Testing probe..." << std::endl;
    
    auto node = std::make_shared<nengo::Node>(1);
    auto probe = std::make_shared<nengo::Probe>(node);
    
    ASSERT_TRUE(probe->getTarget() == node);
    ASSERT_EQ(probe->getData().size(), 0);
    
    // Test recording
    Eigen::VectorXd data1(1);
    data1 << 1.0;
    probe->record(data1);
    
    Eigen::VectorXd data2(1);
    data2 << 2.0;
    probe->record(data2);
    
    ASSERT_EQ(probe->getData().size(), 2);
    ASSERT_TRUE(probe->getData()[0].isApprox(data1));
    ASSERT_TRUE(probe->getData()[1].isApprox(data2));
    
    // Test matrix conversion
    auto matrix = probe->getDataMatrix();
    ASSERT_EQ(matrix.rows(), 2);
    ASSERT_EQ(matrix.cols(), 1);
    ASSERT_EQ(matrix(0, 0), 1.0);
    ASSERT_EQ(matrix(1, 0), 2.0);
    
    std::cout << "Probe test passed!" << std::endl;
    return 0;
}

int main() {
    std::cout << "Running basic Nengo C++ tests..." << std::endl;
    
    int result = 0;
    result |= test_neuron_types();
    result |= test_ensemble();
    result |= test_node();
    result |= test_connection();
    result |= test_probe();
    
    if (result == 0) {
        std::cout << "All basic tests passed!" << std::endl;
    } else {
        std::cout << "Some tests failed!" << std::endl;
    }
    
    return result;
}