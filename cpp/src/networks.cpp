#include "nengo/networks.hpp"
#include "nengo/exceptions.hpp"
#include "nengo/neurons.hpp"
#include <iostream>
#include <sstream>

namespace nengo {
namespace networks {

EnsembleArray::EnsembleArray(int n_neurons,
                            int n_ensembles,
                            int ens_dimensions,
                            const std::string& label,
                            std::optional<int> seed)
    : Network(label, seed),
      n_neurons_per_ensemble_(n_neurons),
      n_ensembles_(n_ensembles),
      dimensions_per_ensemble_(ens_dimensions),
      neuron_input_(nullptr),
      neuron_output_(nullptr) {
      
    if (n_neurons <= 0) {
        throw NengoException("n_neurons must be positive");
    }
    if (n_ensembles <= 0) {
        throw NengoException("n_ensembles must be positive");
    }
    if (ens_dimensions <= 0) {
        throw NengoException("ens_dimensions must be positive");
    }

    std::string label_prefix = label.empty() ? "" : label + "_";

    // Create input node
    input_ = std::make_shared<Node>(getDimensions(), "input");
    add(input_);

    // Create ensembles
    ea_ensembles_.reserve(n_ensembles);
    for (int i = 0; i < n_ensembles; ++i) {
        auto ensemble = std::make_shared<Ensemble>(
            n_neurons,
            ens_dimensions,
            1.0, // radius
            nullptr, // neuron_type (uses default)
            label_prefix + std::to_string(i)
        );
        
        // Create connection from input to this ensemble
        // Note: Simplified version - in a full implementation we'd handle input slicing
        auto conn = std::make_shared<Connection>(
            input_, ensemble
        );
        
        add(ensemble);
        add(conn);
        ea_ensembles_.push_back(ensemble);
    }

    // Create default output
    output_ = addOutput("output", nullptr, nullptr);
}

std::shared_ptr<Node> EnsembleArray::addNeuronInput() {
    if (neuron_input_ != nullptr) {
        std::cerr << "Warning: neuron_input already exists. Returning existing." << std::endl;
        return neuron_input_;
    }

    // Check if ensembles use Direct neurons (cannot provide neuron input to Direct neurons)
    if (ea_ensembles_.size() > 0) {
        auto neuron_type = ea_ensembles_[0]->getNeuronType();
        if (auto direct = std::dynamic_pointer_cast<Direct>(neuron_type)) {
            throw NengoException(
                "Ensembles use Direct neuron type. "
                "Cannot give neuron input to Direct neurons."
            );
        }
    }

    neuron_input_ = std::make_shared<Node>(
        n_neurons_per_ensemble_ * n_ensembles_,
        "neuron_input"
    );
    add(neuron_input_);

    // Connect neuron input to each ensemble's neurons
    // Note: Simplified version - in a full implementation we'd handle neuron-level connections
    for (int i = 0; i < n_ensembles_; ++i) {
        auto conn = std::make_shared<Connection>(neuron_input_, ea_ensembles_[i]);
        add(conn);
    }
    
    return neuron_input_;
}

std::shared_ptr<Node> EnsembleArray::addNeuronOutput() {
    if (neuron_output_ != nullptr) {
        std::cerr << "Warning: neuron_output already exists. Returning existing." << std::endl;
        return neuron_output_;
    }

    // Check if ensembles use Direct neurons (cannot get neuron output from Direct neurons)
    if (ea_ensembles_.size() > 0) {
        auto neuron_type = ea_ensembles_[0]->getNeuronType();
        if (auto direct = std::dynamic_pointer_cast<Direct>(neuron_type)) {
            throw NengoException(
                "Ensembles use Direct neuron type. "
                "Cannot get neuron output from Direct neurons."
            );
        }
    }

    neuron_output_ = std::make_shared<Node>(
        n_neurons_per_ensemble_ * n_ensembles_,
        "neuron_output"
    );
    add(neuron_output_);

    // Connect each ensemble's neurons to neuron output
    // Note: Simplified version - in a full implementation we'd handle neuron-level connections
    for (int i = 0; i < n_ensembles_; ++i) {
        auto conn = std::make_shared<Connection>(ea_ensembles_[i], neuron_output_);
        add(conn);
    }
    
    return neuron_output_;
}

std::shared_ptr<Node> EnsembleArray::addOutput(const std::string& name,
                                              std::function<Eigen::VectorXd(const Eigen::VectorXd&)> function,
                                              std::shared_ptr<Synapse> synapse) {
    
    int total_size;
    if (function == nullptr) {
        // Identity function - output size is same as total dimensions
        total_size = getDimensions();
    } else {
        // Estimate output size by calling function with zero vector
        Eigen::VectorXd test_input = Eigen::VectorXd::Zero(dimensions_per_ensemble_);
        auto test_output = function(test_input);
        total_size = test_output.size() * n_ensembles_;
    }

    auto output_node = std::make_shared<Node>(total_size, name);
    add(output_node);

    // Connect each ensemble to the output node
    // Note: Simplified version - in a full implementation we'd handle output slicing properly
    for (int i = 0; i < n_ensembles_; ++i) {
        auto conn = std::make_shared<Connection>(
            ea_ensembles_[i], output_node,
            function // transform function
        );
        add(conn);
    }

    return output_node;
}

std::string EnsembleArray::toString() const {
    std::ostringstream ss;
    ss << "EnsembleArray(n_neurons=" << n_neurons_per_ensemble_
       << ", n_ensembles=" << n_ensembles_
       << ", ens_dimensions=" << dimensions_per_ensemble_
       << ", label='" << getLabel() << "')";
    return ss.str();
}

// Product implementation
Product::Product(int n_neurons,
                int dimensions,
                double input_magnitude,
                const std::string& label,
                std::optional<int> seed)
    : Network(label, seed),
      dimensions_(dimensions) {
      
    if (n_neurons <= 0) {
        throw NengoException("n_neurons must be positive");
    }
    if (dimensions <= 0) {
        throw NengoException("dimensions must be positive");
    }
    if (input_magnitude <= 0) {
        throw NengoException("input_magnitude must be positive");
    }

    // Create input nodes
    input_a_ = std::make_shared<Node>(dimensions, "input_a");
    input_b_ = std::make_shared<Node>(dimensions, "input_b");
    add(input_a_);
    add(input_b_);

    // Create output node
    output_ = std::make_shared<Node>(dimensions, "output");
    add(output_);

    // Create ensemble arrays for squaring operations
    // Using radius of 2*input_magnitude to handle (a+b) and (a-b) ranges
    double radius = 2.0 * input_magnitude;
    
    sq1_ = std::make_shared<EnsembleArray>(n_neurons, dimensions, 1, "sq1", seed);
    sq2_ = std::make_shared<EnsembleArray>(n_neurons, dimensions, 1, "sq2", seed);
    
    add(sq1_);
    add(sq2_);

    // Define transformation functions for the product computation
    // This is a simplified implementation of the product network
    auto square_func = [](const Eigen::VectorXd& x) -> Eigen::VectorXd {
        // Square function: x -> x^2
        Eigen::VectorXd result(x.size());
        for (int i = 0; i < x.size(); ++i) {
            result[i] = x[i] * x[i];
        }
        return result;
    };

    // Connect inputs to squaring ensembles
    // This is simplified - a full implementation would properly handle the
    // (a+b)^2 - (a-b)^2 = 4ab formula with appropriate transformations
    auto conn_a_sq1 = std::make_shared<Connection>(input_a_, sq1_->getInput());
    auto conn_b_sq1 = std::make_shared<Connection>(input_b_, sq1_->getInput());
    auto conn_a_sq2 = std::make_shared<Connection>(input_a_, sq2_->getInput());
    auto conn_b_sq2 = std::make_shared<Connection>(input_b_, sq2_->getInput());
    
    add(conn_a_sq1);
    add(conn_b_sq1);
    add(conn_a_sq2);
    add(conn_b_sq2);

    // Connect squaring outputs to final output
    // This is simplified - would need proper implementation of the product formula
    auto conn_sq1_out = std::make_shared<Connection>(sq1_->getOutput(), output_);
    auto conn_sq2_out = std::make_shared<Connection>(sq2_->getOutput(), output_);
    add(conn_sq1_out);
    add(conn_sq2_out);
}

std::string Product::toString() const {
    std::ostringstream ss;
    ss << "Product(dimensions=" << dimensions_
       << ", label='" << getLabel() << "')";
    return ss.str();
}

} // namespace networks
} // namespace nengo