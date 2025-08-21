#pragma once

#include "nengo/base.hpp"
#include <Eigen/Dense>
#include <functional>
#include <memory>

namespace nengo {

// Forward declarations
class Ensemble;
class Node;

/**
 * Connections are used to link objects together.
 * They can apply transforms and functions to the data being transmitted.
 */
class Connection : public NengoObject {
public:
    using TransformFunction = std::function<Eigen::VectorXd(const Eigen::VectorXd&)>;

    /**
     * Create a connection between two objects.
     * @param pre Pre-synaptic object (Ensemble or Node)
     * @param post Post-synaptic object (Ensemble or Node)
     * @param function Function to apply to the data
     * @param transform Linear transformation matrix
     * @param synapse Synaptic filter (not implemented in basic version)
     * @param label Label for the connection
     */
    Connection(std::shared_ptr<NengoObject> pre,
               std::shared_ptr<NengoObject> post,
               TransformFunction function = nullptr,
               const Eigen::MatrixXd& transform = Eigen::MatrixXd(),
               const std::string& label = "");

    ~Connection() override = default;

    // Getters
    std::shared_ptr<NengoObject> getPre() const { return pre_; }
    std::shared_ptr<NengoObject> getPost() const { return post_; }
    bool hasFunction() const { return function_ != nullptr; }
    const Eigen::MatrixXd& getTransform() const { return transform_; }

    // Compute connection output
    Eigen::VectorXd compute(const Eigen::VectorXd& input) const;

    // Get dimensions
    int getPreDimensions() const;
    int getPostDimensions() const;

private:
    std::shared_ptr<NengoObject> pre_;
    std::shared_ptr<NengoObject> post_;
    TransformFunction function_;
    Eigen::MatrixXd transform_;
    
    void initializeTransform();
};

} // namespace nengo