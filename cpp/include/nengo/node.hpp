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
 * Equivalent to nengo.node.Node in Python version.
 */
class Node : public NengoObject {
public:
    using OutputFunction = std::function<Eigen::VectorXd(double, const Eigen::VectorXd&)>;

    /**
     * Constructor for Node matching Python signature.
     * @param output Function that computes output or constant output vector
     * @param size_in Number of input dimensions (default: 0)
     * @param size_out Number of output dimensions (default: None, determined automatically)
     * @param label Label for the node
     * @param seed Random number seed (currently unused like in Python)
     */
    explicit Node(OutputFunction output = nullptr,
                  int size_in = 0,
                  std::optional<int> size_out = std::nullopt,
                  const std::string& label = "",
                  std::optional<int> seed = std::nullopt);

    /**
     * Constructor for constant output Node.
     */
    explicit Node(const Eigen::VectorXd& output,
                  int size_in = 0,
                  const std::string& label = "",
                  std::optional<int> seed = std::nullopt);

    /**
     * Constructor for passthrough Node (identity function).
     */
    explicit Node(int size_in, 
                  const std::string& label = "",
                  std::optional<int> seed = std::nullopt);

    ~Node() override = default;

    // Getters matching Python properties
    int getSizeIn() const { return size_in_; }
    int getSizeOut() const { return size_out_; }
    bool hasOutput() const { return output_function_ != nullptr; }

    // Compute output
    Eigen::VectorXd run(double t, const Eigen::VectorXd& input = Eigen::VectorXd()) const;

    // String representation
    std::string toString() const override;

private:
    OutputFunction output_function_;
    int size_in_;
    int size_out_;
    Eigen::VectorXd constant_output_;
    bool is_constant_;
    bool is_passthrough_;

    void initializeAsPassthrough();
    void initializeAsConstant(const Eigen::VectorXd& output);
    void determineSizeOut(std::optional<int> size_out_param);
};

} // namespace nengo