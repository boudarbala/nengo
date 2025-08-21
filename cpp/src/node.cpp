#include "nengo/node.hpp"
#include <sstream>

namespace nengo {

Node::Node(OutputFunction output, int size_in, std::optional<int> size_out, 
           const std::string& label, std::optional<int> seed)
    : NengoObject(label.empty() ? "Node" : label, seed),
      output_function_(output),
      size_in_(size_in),
      is_constant_(false),
      is_passthrough_(false) {
    
    determineSizeOut(size_out);
    
    if (!output_function_) {
        initializeAsPassthrough();
    }
}

Node::Node(const Eigen::VectorXd& output, int size_in, 
           const std::string& label, std::optional<int> seed)
    : NengoObject(label.empty() ? "Node" : label, seed),
      size_in_(size_in),
      size_out_(output.size()),
      is_constant_(true),
      is_passthrough_(false) {
    
    initializeAsConstant(output);
}

Node::Node(int size_in, const std::string& label, std::optional<int> seed)
    : NengoObject(label.empty() ? "Node" : label, seed),
      size_in_(size_in),
      size_out_(size_in),
      is_constant_(false),
      is_passthrough_(true) {
    
    initializeAsPassthrough();
}

std::string Node::toString() const {
    std::ostringstream oss;
    oss << "<Node '" << getLabel() << "' size_in=" << size_in_ 
        << " size_out=" << size_out_ << ">";
    return oss.str();
}

Eigen::VectorXd Node::run(double t, const Eigen::VectorXd& input) const {
    if (is_constant_) {
        return constant_output_;
    } else if (is_passthrough_) {
        if (input.size() != size_in_) {
            throw std::invalid_argument("Input size must match node size_in");
        }
        return input;
    } else if (output_function_) {
        return output_function_(t, input);
    } else {
        // Default: return zeros
        return Eigen::VectorXd::Zero(size_out_);
    }
}

void Node::determineSizeOut(std::optional<int> size_out_param) {
    if (size_out_param.has_value()) {
        size_out_ = size_out_param.value();
    } else {
        // Default size_out determination logic
        if (size_in_ == 0) {
            size_out_ = 1;  // Default for input nodes
        } else {
            size_out_ = size_in_;  // Passthrough default
        }
    }
}

void Node::initializeAsPassthrough() {
    is_passthrough_ = true;
    output_function_ = [this](double t, const Eigen::VectorXd& x) -> Eigen::VectorXd {
        return x;
    };
}

void Node::initializeAsConstant(const Eigen::VectorXd& output) {
    constant_output_ = output;
    output_function_ = [this](double t, const Eigen::VectorXd& x) -> Eigen::VectorXd {
        return constant_output_;
    };
}

} // namespace nengo