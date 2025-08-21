#include "nengo/ensemble.hpp"
#include "nengo/neurons.hpp"
#include <random>

namespace nengo {

Ensemble::Ensemble(int n_neurons, int dimensions, double radius,
                   std::shared_ptr<NeuronType> neuron_type,
                   const std::string& label)
    : NengoObject(label.empty() ? "Ensemble" : label),
      n_neurons_(n_neurons),
      dimensions_(dimensions),
      radius_(radius),
      neuron_type_(neuron_type ? neuron_type : std::make_shared<LIFRate>()),
      neurons(this) {
    
    initializeDefaults();
}

void Ensemble::setEncoders(const Eigen::MatrixXd& encoders) {
    if (encoders.rows() != n_neurons_ || encoders.cols() != dimensions_) {
        throw std::invalid_argument("Encoder dimensions must match (n_neurons, dimensions)");
    }
    encoders_ = encoders;
}

void Ensemble::setGainBias(const Eigen::MatrixXd& gain, const Eigen::VectorXd& bias) {
    if (gain.rows() != n_neurons_ || bias.size() != n_neurons_) {
        throw std::invalid_argument("Gain and bias dimensions must match n_neurons");
    }
    gain_ = gain;
    bias_ = bias;
}

void Ensemble::initializeDefaults() {
    // Initialize random encoders (unit vectors)
    encoders_ = Eigen::MatrixXd::Random(n_neurons_, dimensions_);
    for (int i = 0; i < n_neurons_; ++i) {
        encoders_.row(i).normalize();
    }
    
    // Initialize default gain and bias
    Eigen::VectorXd max_rates = Eigen::VectorXd::Constant(n_neurons_, 200.0); // 200 Hz
    Eigen::VectorXd intercepts = Eigen::VectorXd::Random(n_neurons_) * 0.9; // [-0.9, 0.9]
    
    Eigen::VectorXd gain_vec(n_neurons_);
    neuron_type_->gainBias(max_rates, intercepts, gain_vec, bias_);
    gain_ = gain_vec.asDiagonal();
}

} // namespace nengo