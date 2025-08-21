#pragma once

#include "nengo/base.hpp"
#include "nengo/synapses.hpp"
#include <Eigen/Dense>
#include <random>
#include <functional>

namespace nengo {

/**
 * A white noise process for generating random signals.
 */
class WhiteNoise : public Process {
public:
    /**
     * Constructor for WhiteNoise process.
     * 
     * Parameters
     * ----------
     * mean : double, optional
     *     Mean of the Gaussian distribution. Default is 0.
     * std : double, optional
     *     Standard deviation of the Gaussian distribution. Default is 1.
     * scale : bool, optional
     *     Whether to scale the white noise for integration. Default is true.
     * seed : int, optional
     *     Random number seed. Default is random.
     */
    explicit WhiteNoise(double mean = 0.0, double std = 1.0, 
                       bool scale = true, std::optional<int> seed = std::nullopt);
    
    /**
     * Generate a step of white noise.
     * 
     * Parameters
     * ----------
     * dt : double
     *     Time step size.
     * shape_out : int
     *     Number of output dimensions.
     * 
     * Returns
     * -------
     * Eigen::VectorXd
     *     White noise sample.
     */
    Eigen::VectorXd step(double dt, int shape_out);
    
    // Getters
    double getMean() const { return mean_; }
    double getStd() const { return std_; }
    bool getScale() const { return scale_; }

private:
    double mean_;
    double std_;
    bool scale_;
    std::mt19937 rng_;
    std::normal_distribution<double> distribution_;
};

/**
 * Filtered white noise process.
 * 
 * This process takes white noise and filters it using the provided synapse.
 */
class FilteredNoise : public Process {
public:
    /**
     * Constructor for FilteredNoise process.
     * 
     * Parameters
     * ----------
     * synapse : std::shared_ptr<Synapse>
     *     The synapse to use to filter the noise.
     * mean : double, optional
     *     Mean of the Gaussian distribution. Default is 0.
     * std : double, optional
     *     Standard deviation of the Gaussian distribution. Default is 1.
     * scale : bool, optional
     *     Whether to scale the white noise for integration. Default is true.
     * seed : int, optional
     *     Random number seed. Default is random.
     */
    explicit FilteredNoise(std::shared_ptr<Synapse> synapse,
                          double mean = 0.0, double std = 1.0,
                          bool scale = true, std::optional<int> seed = std::nullopt);
    
    /**
     * Generate a step of filtered noise.
     * 
     * Parameters
     * ----------
     * dt : double
     *     Time step size.
     * shape_out : int
     *     Number of output dimensions.
     * 
     * Returns
     * -------
     * Eigen::VectorXd
     *     Filtered noise sample.
     */
    Eigen::VectorXd step(double dt, int shape_out);
    
    // Getters
    std::shared_ptr<Synapse> getSynapse() const { return synapse_; }
    double getMean() const { return noise_->getMean(); }
    double getStd() const { return noise_->getStd(); }
    bool getScale() const { return noise_->getScale(); }

private:
    std::shared_ptr<Synapse> synapse_;
    std::unique_ptr<WhiteNoise> noise_;
    Eigen::VectorXd state_;
    bool state_initialized_;
};

/**
 * A piecewise constant process defined by a set of time points and values.
 */
class PiecewiseConstant : public Process {
public:
    /**
     * Constructor for PiecewiseConstant process.
     * 
     * Parameters
     * ----------
     * times : std::vector<double>
     *     Time points where the function changes value.
     * values : std::vector<Eigen::VectorXd>
     *     Values at each time point.
     * interpolate : bool, optional
     *     Whether to interpolate between values. Default is false.
     */
    PiecewiseConstant(const std::vector<double>& times,
                     const std::vector<Eigen::VectorXd>& values,
                     bool interpolate = false);
    
    /**
     * Get the value at a specific time.
     * 
     * Parameters
     * ----------
     * t : double
     *     Current time.
     * 
     * Returns
     * -------
     * Eigen::VectorXd
     *     Value at time t.
     */
    Eigen::VectorXd getValue(double t) const;
    
    // Getters
    const std::vector<double>& getTimes() const { return times_; }
    const std::vector<Eigen::VectorXd>& getValues() const { return values_; }
    bool getInterpolate() const { return interpolate_; }

private:
    std::vector<double> times_;
    std::vector<Eigen::VectorXd> values_;
    bool interpolate_;
    
    size_t findTimeIndex(double t) const;
};

/**
 * A sinusoidal process.
 */
class Sine : public Process {
public:
    /**
     * Constructor for Sine process.
     * 
     * Parameters
     * ----------
     * frequency : double
     *     Frequency of the sine wave in Hz.
     * amplitude : double, optional
     *     Amplitude of the sine wave. Default is 1.0.
     * phase : double, optional
     *     Phase offset in radians. Default is 0.0.
     */
    explicit Sine(double frequency, double amplitude = 1.0, double phase = 0.0);
    
    /**
     * Get the value at a specific time.
     * 
     * Parameters
     * ----------
     * t : double
     *     Current time.
     * 
     * Returns
     * -------
     * double
     *     Sine value at time t.
     */
    double getValue(double t) const;
    
    // Getters
    double getFrequency() const { return frequency_; }
    double getAmplitude() const { return amplitude_; }
    double getPhase() const { return phase_; }

private:
    double frequency_;
    double amplitude_;
    double phase_;
};

} // namespace nengo