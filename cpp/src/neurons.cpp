#include "nengo/neurons.hpp"
#include <cmath>
#include <algorithm>
#include <memory>

namespace nengo {

// Direct neuron implementation
void Direct::step(const Eigen::VectorXd& x, Eigen::VectorXd& output) const {
    output = x;
}

void Direct::gainBias(const Eigen::VectorXd& max_rates,
                      const Eigen::VectorXd& intercepts,
                      Eigen::VectorXd& gain,
                      Eigen::VectorXd& bias) const {
    // For Direct neurons, gain and bias don't matter much
    gain = Eigen::VectorXd::Ones(max_rates.size());
    bias = Eigen::VectorXd::Zero(intercepts.size());
}

std::unique_ptr<NeuronType> Direct::clone() const {
    return std::make_unique<Direct>();
}

// LIF neuron implementation
LIF::LIF(double tau_ref, double tau_rc, double amplitude)
    : tau_ref_(tau_ref), tau_rc_(tau_rc), amplitude_(amplitude) {}

void LIF::step(const Eigen::VectorXd& x, Eigen::VectorXd& output) const {
    output.resize(x.size());
    for (int i = 0; i < x.size(); ++i) {
        if (x(i) <= 0) {
            output(i) = 0.0;
        } else {
            output(i) = amplitude_ / (tau_ref_ + tau_rc_ * std::log1p(1.0 / x(i)));
        }
    }
}

void LIF::gainBias(const Eigen::VectorXd& max_rates,
                   const Eigen::VectorXd& intercepts,
                   Eigen::VectorXd& gain,
                   Eigen::VectorXd& bias) const {
    gain.resize(max_rates.size());
    bias.resize(intercepts.size());
    
    for (int i = 0; i < max_rates.size(); ++i) {
        double max_rate = max_rates(i);
        double intercept = intercepts(i);
        
        if (max_rate > 0) {
            double x1 = intercept;
            double x2 = 1.0;
            double z1 = 1.0 / (1.0 - std::exp((tau_ref_ - 1.0 / max_rate) / tau_rc_));
            double z2 = 1.0 / (1.0 - std::exp((tau_ref_ - 1.0 / max_rate) / tau_rc_));
            
            gain(i) = (z1 - z2) / (x1 - x2);
            bias(i) = z1 - gain(i) * x1;
        } else {
            gain(i) = 0.0;
            bias(i) = 0.0;
        }
    }
}

std::unique_ptr<NeuronType> LIF::clone() const {
    return std::make_unique<LIF>(tau_ref_, tau_rc_, amplitude_);
}

// LIFRate neuron implementation
LIFRate::LIFRate(double tau_ref, double tau_rc, double amplitude)
    : tau_ref_(tau_ref), tau_rc_(tau_rc), amplitude_(amplitude) {}

void LIFRate::step(const Eigen::VectorXd& x, Eigen::VectorXd& output) const {
    output.resize(x.size());
    for (int i = 0; i < x.size(); ++i) {
        if (x(i) <= 1.0) {
            output(i) = 0.0;
        } else {
            output(i) = amplitude_ / (tau_ref_ + tau_rc_ * std::log(x(i) / (x(i) - 1.0)));
        }
    }
}

void LIFRate::gainBias(const Eigen::VectorXd& max_rates,
                       const Eigen::VectorXd& intercepts,
                       Eigen::VectorXd& gain,
                       Eigen::VectorXd& bias) const {
    gain.resize(max_rates.size());
    bias.resize(intercepts.size());
    
    for (int i = 0; i < max_rates.size(); ++i) {
        double max_rate = max_rates(i);
        double intercept = intercepts(i);
        
        if (max_rate > 0) {
            double inv_max_rate = 1.0 / max_rate;
            double x1 = intercept;
            double x2 = 1.0;
            
            if (inv_max_rate > tau_ref_) {
                double z1 = 1.0 / (1.0 - std::exp((tau_ref_ - inv_max_rate) / tau_rc_));
                double z2 = 1.0 / (1.0 - std::exp((tau_ref_ - inv_max_rate) / tau_rc_));
                
                gain(i) = (z1 - z2) / (x1 - x2);
                bias(i) = z1 - gain(i) * x1;
            } else {
                gain(i) = 0.0;
                bias(i) = 1.0;
            }
        } else {
            gain(i) = 0.0;
            bias(i) = 0.0;
        }
    }
}

std::unique_ptr<NeuronType> LIFRate::clone() const {
    return std::make_unique<LIFRate>(tau_ref_, tau_rc_, amplitude_);
}

} // namespace nengo