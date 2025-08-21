#include "nengo/processes.hpp"
#include <cmath>
#include <stdexcept>
#include <algorithm>

namespace nengo {

// WhiteNoise Implementation

WhiteNoise::WhiteNoise(double mean, double std, bool scale, std::optional<int> seed)
    : Process(0, 1, 0.001, seed), mean_(mean), std_(std), scale_(scale) {
    
    if (seed.has_value()) {
        rng_.seed(seed.value());
    } else {
        std::random_device rd;
        rng_.seed(rd());
    }
    
    distribution_ = std::normal_distribution<double>(mean_, std_);
}

Eigen::VectorXd WhiteNoise::step(double dt, int shape_out) {
    Eigen::VectorXd result(shape_out);
    
    double alpha = scale_ ? 1.0 / std::sqrt(dt) : 1.0;
    
    for (int i = 0; i < shape_out; ++i) {
        result(i) = alpha * distribution_(rng_);
    }
    
    return result;
}

// FilteredNoise Implementation

FilteredNoise::FilteredNoise(std::shared_ptr<Synapse> synapse,
                           double mean, double std, bool scale, std::optional<int> seed)
    : Process(0, 1, 0.001, seed), synapse_(synapse), state_initialized_(false) {
    
    noise_ = std::make_unique<WhiteNoise>(mean, std, scale, seed);
}

Eigen::VectorXd FilteredNoise::step(double dt, int shape_out) {
    // Get white noise sample
    Eigen::VectorXd noise_sample = noise_->step(dt, shape_out);
    
    // Initialize state if needed
    if (!state_initialized_) {
        state_ = Eigen::VectorXd::Zero(shape_out);
        state_initialized_ = true;
    }
    
    // Filter the noise through the synapse
    // This is a simplified implementation - in practice, the synapse would
    // maintain its own state and filtering logic
    if (synapse_) {
        // Apply simple first-order filtering as approximation
        double alpha = dt / (synapse_->getTau() + dt);
        state_ = (1.0 - alpha) * state_ + alpha * noise_sample;
        return state_;
    }
    
    return noise_sample;
}

// PiecewiseConstant Implementation

PiecewiseConstant::PiecewiseConstant(const std::vector<double>& times,
                                   const std::vector<Eigen::VectorXd>& values,
                                   bool interpolate)
    : Process(0, values.empty() ? 1 : values[0].size(), 0.001),
      times_(times), values_(values), interpolate_(interpolate) {
    
    if (times.size() != values.size()) {
        throw std::invalid_argument("Times and values must have the same size");
    }
    
    if (times.empty()) {
        throw std::invalid_argument("Times and values cannot be empty");
    }
    
    // Check that times are in non-decreasing order
    for (size_t i = 1; i < times.size(); ++i) {
        if (times[i] < times[i-1]) {
            throw std::invalid_argument("Times must be in non-decreasing order");
        }
    }
}

size_t PiecewiseConstant::findTimeIndex(double t) const {
    // Find the largest index where times_[index] <= t
    auto it = std::upper_bound(times_.begin(), times_.end(), t);
    if (it == times_.begin()) {
        return 0;
    }
    return std::distance(times_.begin(), it) - 1;
}

Eigen::VectorXd PiecewiseConstant::getValue(double t) const {
    if (t < times_[0]) {
        return values_[0];
    }
    
    if (t >= times_.back()) {
        return values_.back();
    }
    
    size_t idx = findTimeIndex(t);
    
    if (!interpolate_ || idx >= times_.size() - 1) {
        return values_[idx];
    }
    
    // Linear interpolation
    double t0 = times_[idx];
    double t1 = times_[idx + 1];
    double alpha = (t - t0) / (t1 - t0);
    
    return (1.0 - alpha) * values_[idx] + alpha * values_[idx + 1];
}

// Sine Implementation

Sine::Sine(double frequency, double amplitude, double phase)
    : Process(0, 1, 0.001), frequency_(frequency), amplitude_(amplitude), phase_(phase) {
    
    if (frequency < 0) {
        throw std::invalid_argument("Frequency must be non-negative");
    }
}

double Sine::getValue(double t) const {
    return amplitude_ * std::sin(2.0 * M_PI * frequency_ * t + phase_);
}

} // namespace nengo