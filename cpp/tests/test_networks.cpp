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

int test_ensemble_array() {
    std::cout << "Testing EnsembleArray..." << std::endl;
    
    auto ea = std::make_shared<nengo::networks::EnsembleArray>(50, 3, 2, "test_ea");
    
    ASSERT_EQ(ea->getNEnsembles(), 3);
    ASSERT_EQ(ea->getDimensionsPerEnsemble(), 2);
    ASSERT_EQ(ea->getDimensions(), 6); // 3 * 2
    ASSERT_EQ(ea->getNeuronsPerEnsemble(), 50);
    
    ASSERT_TRUE(ea->getInput() != nullptr);
    ASSERT_TRUE(ea->getOutput() != nullptr);
    ASSERT_EQ(ea->getEaEnsembles().size(), 3);
    
    std::cout << "EnsembleArray toString: " << ea->toString() << std::endl;
    
    std::cout << "EnsembleArray test passed!" << std::endl;
    return 0;
}

int test_product() {
    std::cout << "Testing Product..." << std::endl;
    
    auto product = std::make_shared<nengo::networks::Product>(100, 2, 1.0, "test_product");
    
    ASSERT_EQ(product->getDimensions(), 2);
    ASSERT_TRUE(product->getInputA() != nullptr);
    ASSERT_TRUE(product->getInputB() != nullptr);
    ASSERT_TRUE(product->getOutput() != nullptr);
    
    std::cout << "Product toString: " << product->toString() << std::endl;
    
    std::cout << "Product test passed!" << std::endl;
    return 0;
}

int main() {
    std::cout << "Running Nengo C++ networks tests..." << std::endl;
    
    int result = 0;
    result |= test_ensemble_array();
    result |= test_product();
    
    if (result == 0) {
        std::cout << "All networks tests passed!" << std::endl;
    } else {
        std::cout << "Some tests failed!" << std::endl;
    }
    
    return result;
}