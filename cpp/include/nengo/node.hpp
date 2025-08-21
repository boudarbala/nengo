#pragma once

#include "nengo/base.hpp"
#include <Eigen/Dense>
#include <functional>

namespace nengo {

/**
 * Nodes are non-neural components of a Nengo model.
 * They can be used to provide input to a model, take output from a model,
 * or to implement non-neural transformations for compatibility with 
 * non-neural models.
 */
class Node : public NengoObject {
public:
    using OutputFunction = std::function<Eigen::VectorXd(double, const Eigen::VectorXd&)>;

    /**
     * Constructor for Node.
     * @param output Function that computes output or constant output vector
     * @param size_in Number of input dimensions (0 for no input)
     * @param size_out Number of output dimensions
     * @param label Label for the node
     */
    explicit Node(OutputFunction output = nullptr,
                  int size_in = 0,
                  int size_out = 1,
                  const std::string& label = "");

    /**
     * Constructor for constant output Node.
     */
    explicit Node(const Eigen::VectorXd& output,
                  int size_in = 0,
                  const std::string& label = "");

    /**
     * Constructor for passthrough Node (identity function).
     */
    explicit Node(int size_in, const std::string& label = "");

    ~Node() override = default;

    // Getters
    int getSizeIn() const { return size_in_; }
    int getSizeOut() const { return size_out_; }
    bool hasOutput() const { return output_function_ != nullptr; }

    // Compute output
    Eigen::VectorXd run(double t, const Eigen::VectorXd& input = Eigen::VectorXd()) const;

private:
    OutputFunction output_function_;
    int size_in_;
    int size_out_;
    Eigen::VectorXd constant_output_;
    bool is_constant_;
    bool is_passthrough_;

    void initializeAsPassthrough();
    void initializeAsConstant(const Eigen::VectorXd& output);
};

} // namespace nengo