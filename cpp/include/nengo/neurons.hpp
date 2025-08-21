#pragma once

#include <Eigen/Dense>
#include <functional>
#include <memory>

namespace nengo {

/**
 * Base class for neuron types.
 * Defines the interface for different neuron models.
 */
class NeuronType {
public:
    virtual ~NeuronType() = default;
    
    /**
     * Compute neuron activities given current input.
     * @param x Input current to neurons (n_neurons,)
     * @param output Output activities (n_neurons,)
     */
    virtual void step(const Eigen::VectorXd& x, Eigen::VectorXd& output) const = 0;
    
    /**
     * Get the gain and bias for neurons given max_rates and intercepts.
     */
    virtual void gainBias(const Eigen::VectorXd& max_rates,
                         const Eigen::VectorXd& intercepts,
                         Eigen::VectorXd& gain,
                         Eigen::VectorXd& bias) const = 0;
                         
    virtual std::unique_ptr<NeuronType> clone() const = 0;
};

/**
 * Direct neuron type - no actual neurons, just passes values through.
 * Equivalent to nengo.neurons.Direct in Python version.
 */
class Direct : public NeuronType {
public:
    void step(const Eigen::VectorXd& x, Eigen::VectorXd& output) const override;
    
    void gainBias(const Eigen::VectorXd& max_rates,
                  const Eigen::VectorXd& intercepts,
                  Eigen::VectorXd& gain,
                  Eigen::VectorXd& bias) const override;
                  
    std::unique_ptr<NeuronType> clone() const override;
};

/**
 * Leaky Integrate-and-Fire (LIF) neuron model.
 * Equivalent to nengo.neurons.LIF in Python version.
 */
class LIF : public NeuronType {
public:
    explicit LIF(double tau_ref = 0.002, double tau_rc = 0.02, double amplitude = 1.0);
    
    void step(const Eigen::VectorXd& x, Eigen::VectorXd& output) const override;
    
    void gainBias(const Eigen::VectorXd& max_rates,
                  const Eigen::VectorXd& intercepts,
                  Eigen::VectorXd& gain,
                  Eigen::VectorXd& bias) const override;
                  
    std::unique_ptr<NeuronType> clone() const override;
    
    double getTauRef() const { return tau_ref_; }
    double getTauRc() const { return tau_rc_; }
    double getAmplitude() const { return amplitude_; }

private:
    double tau_ref_;   // Refractory period
    double tau_rc_;    // RC time constant
    double amplitude_; // Scaling factor
};

/**
 * LIF rate neuron - approximates LIF neuron firing rates.
 */
class LIFRate : public NeuronType {
public:
    explicit LIFRate(double tau_ref = 0.002, double tau_rc = 0.02, double amplitude = 1.0);
    
    void step(const Eigen::VectorXd& x, Eigen::VectorXd& output) const override;
    
    void gainBias(const Eigen::VectorXd& max_rates,
                  const Eigen::VectorXd& intercepts,
                  Eigen::VectorXd& gain,
                  Eigen::VectorXd& bias) const override;
                  
    std::unique_ptr<NeuronType> clone() const override;

private:
    double tau_ref_;
    double tau_rc_;
    double amplitude_;
};

} // namespace nengo