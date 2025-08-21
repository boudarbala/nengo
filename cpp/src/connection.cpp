#include "nengo/connection.hpp"
#include "nengo/ensemble.hpp"
#include "nengo/node.hpp"
#include <sstream>

namespace nengo {

Connection::Connection(std::shared_ptr<NengoObject> pre,
                       std::shared_ptr<NengoObject> post,
                       TransformFunction function,
                       const Eigen::MatrixXd& transform,
                       const std::string& label,
                       std::optional<int> seed)
    : NengoObject(label.empty() ? "Connection" : label, seed),
      pre_(pre),
      post_(post),
      function_(function),
      transform_(transform) {
    
    initializeTransform();
}

std::string Connection::toString() const {
    std::ostringstream oss;
    oss << "<Connection '" << getLabel() << "' from "
        << pre_->getLabel() << " to " << post_->getLabel() << ">";
    return oss.str();
}

Eigen::VectorXd Connection::compute(const Eigen::VectorXd& input) const {
    Eigen::VectorXd result = input;
    
    // Apply function if provided
    if (function_) {
        result = function_(result);
    }
    
    // Apply transform if provided
    if (transform_.rows() > 0 && transform_.cols() > 0) {
        result = transform_ * result;
    }
    
    return result;
}

int Connection::getPreDimensions() const {
    if (auto ensemble = std::dynamic_pointer_cast<Ensemble>(pre_)) {
        return ensemble->getDimensions();
    } else if (auto node = std::dynamic_pointer_cast<Node>(pre_)) {
        return node->getSizeOut();
    }
    return 1;
}

int Connection::getPostDimensions() const {
    if (auto ensemble = std::dynamic_pointer_cast<Ensemble>(post_)) {
        return ensemble->getDimensions();
    } else if (auto node = std::dynamic_pointer_cast<Node>(post_)) {
        return node->getSizeIn();
    }
    return 1;
}

void Connection::initializeTransform() {
    if (transform_.rows() == 0 || transform_.cols() == 0) {
        int pre_dims = getPreDimensions();
        int post_dims = getPostDimensions();
        
        if (pre_dims == post_dims) {
            // Identity transform
            transform_ = Eigen::MatrixXd::Identity(post_dims, pre_dims);
        } else {
            // Zero transform with appropriate dimensions
            transform_ = Eigen::MatrixXd::Zero(post_dims, pre_dims);
        }
    }
}

} // namespace nengo