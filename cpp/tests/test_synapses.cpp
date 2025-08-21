#include <iostream>
#include <cassert>
#include <cmath>
#include <nengo/nengo.hpp>

void test_lowpass() {
    std::cout << "Testing Lowpass synapse..." << std::endl;
    
    // Create a lowpass filter with tau=0.1
    auto lowpass = std::make_shared<nengo::Lowpass>(0.1);
    
    // Test basic properties
    assert(lowpass->getTau() == 0.1);
    assert(lowpass->getDefaultSizeIn() == 1);
    assert(lowpass->getDefaultSizeOut() == 1);
    
    // Test filtering a step response
    Eigen::VectorXd step_input(10);
    step_input.setOnes();
    
    Eigen::VectorXd filtered = lowpass->filt(step_input, 0.01);
    
    // The output should be smoothed (exponential rise)
    assert(filtered(0) > 0 && filtered(0) < 1.0);
    assert(filtered(9) > filtered(0)); // Should be increasing
    
    std::cout << "Lowpass test passed!" << std::endl;
}

void test_alpha() {
    std::cout << "Testing Alpha synapse..." << std::endl;
    
    // Create an alpha filter with tau=0.1
    auto alpha = std::make_shared<nengo::Alpha>(0.1);
    
    // Test basic properties
    assert(alpha->getTau() == 0.1);
    assert(alpha->getDefaultSizeIn() == 1);
    assert(alpha->getDefaultSizeOut() == 1);
    
    // Test toString
    std::string str = alpha->toString();
    assert(str.find("Alpha") != std::string::npos);
    assert(str.find("0.1") != std::string::npos);
    
    std::cout << "Alpha test passed!" << std::endl;
}

void test_triangle() {
    std::cout << "Testing Triangle synapse..." << std::endl;
    
    // Create a triangle filter with t=0.05
    auto triangle = std::make_shared<nengo::Triangle>(0.05);
    
    // Test basic properties
    assert(triangle->getT() == 0.05);
    assert(triangle->getDefaultSizeIn() == 1);
    assert(triangle->getDefaultSizeOut() == 1);
    
    // Test toString
    std::string str = triangle->toString();
    assert(str.find("Triangle") != std::string::npos);
    assert(str.find("0.05") != std::string::npos);
    
    std::cout << "Triangle test passed!" << std::endl;
}

void test_linear_filter() {
    std::cout << "Testing LinearFilter..." << std::endl;
    
    // Create a simple linear filter
    std::vector<double> num = {1.0};
    std::vector<double> den = {0.1, 1.0}; // tau=0.1, equivalent to Lowpass
    auto filter = std::make_shared<nengo::LinearFilter>(num, den, true);
    
    // Test basic properties
    assert(filter->getNum() == num);
    assert(filter->getDen() == den);
    assert(filter->isAnalog() == true);
    
    // Test toString
    std::string str = filter->toString();
    assert(str.find("LinearFilter") != std::string::npos);
    
    std::cout << "LinearFilter test passed!" << std::endl;
}

void test_synapse_inheritance() {
    std::cout << "Testing synapse inheritance..." << std::endl;
    
    // Test that synapse types can be used polymorphically
    std::vector<std::shared_ptr<nengo::Synapse>> synapses;
    synapses.push_back(std::make_shared<nengo::Lowpass>(0.1));
    synapses.push_back(std::make_shared<nengo::Alpha>(0.05));
    synapses.push_back(std::make_shared<nengo::Triangle>(0.02));
    
    // Test that they all have synapse interface
    for (auto& syn : synapses) {
        assert(syn->getDefaultSizeIn() == 1);
        assert(syn->getDefaultSizeOut() == 1);
        assert(syn->getDefaultDt() == 0.001);
        
        // Test toString works for all
        std::string str = syn->toString();
        assert(str.length() > 0);
    }
    
    std::cout << "Synapse inheritance test passed!" << std::endl;
}

int main() {
    std::cout << "Running Nengo C++ synapse tests..." << std::endl;
    
    try {
        test_lowpass();
        test_alpha();
        test_triangle();
        test_linear_filter();
        test_synapse_inheritance();
        
        std::cout << "All synapse tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}