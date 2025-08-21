#pragma once

#include "nengo/base.hpp" 
#include <Eigen/Dense>
#include <vector>
#include <memory>
#include <functional>

namespace nengo {

/**
 * Abstract base class for synapse models.
 * 
 * Equivalent to nengo.synapses.Synapse in Python version.
 * 
 * Conceptually, a synapse model emulates a biological synapse, taking in
 * input in the form of released neurotransmitter and opening ion channels
 * to allow more or less current to flow into the neuron.
 * 
 * In Nengo, the implementation of a synapse is as a specific case of a
 * Process in which the input and output shapes are the same.
 * The input is the current across the synapse, and the output is the current
 * that will be induced in the postsynaptic neuron.
 * 
 * Parameters
 * ----------
 * default_size_in : int, optional
 *     The size_in used if not specified (default: 1).
 * default_size_out : int, optional
 *     The size_out used if not specified. If None, will be the same as 
 *     default_size_in (default: None).
 * default_dt : double, optional
 *     The simulation timestep used if not specified (default: 0.001).
 * seed : int, optional
 *     Random number seed. Ensures random factors will be the same each run
 *     (default: None).
 * 
 * Attributes
 * ----------
 * default_dt : double
 *     The simulation timestep used if not specified.
 * default_size_in : int
 *     The size_in used if not specified.
 * default_size_out : int
 *     The size_out used if not specified.
 * seed : int or None
 *     Random number seed. Ensures random factors will be the same each run.
 */
class Synapse : public Process {
public:
    explicit Synapse(int default_size_in = 1,
                    int default_size_out = -1, // -1 means same as default_size_in
                    double default_dt = 0.001,
                    std::optional<int> seed = std::nullopt);
    virtual ~Synapse() = default;

    /**
     * Create initial state for the synapse.
     * 
     * @param shape_in Input shape
     * @param shape_out Output shape  
     * @param dt Simulation timestep
     * @return State object for this synapse
     */
    virtual std::shared_ptr<void> makeState(const std::vector<int>& shape_in,
                                           const std::vector<int>& shape_out,
                                           double dt) = 0;

    /**
     * Create step function for the synapse.
     * 
     * @param shape_in Input shape
     * @param shape_out Output shape
     * @param dt Simulation timestep
     * @param state State object for this synapse
     * @return Step function that processes signals
     */
    virtual std::function<Eigen::VectorXd(double, const Eigen::VectorXd&)> 
    makeStep(const std::vector<int>& shape_in,
             const std::vector<int>& shape_out,
             double dt,
             std::shared_ptr<void> state) = 0;

    /**
     * Filter signal x with this synapse model.
     * 
     * @param x The signal to filter
     * @param dt The timestep of the input signal (default: use default_dt)
     * @param y0 The starting state of the filter output (default: 0)
     * @return Filtered signal
     */
    virtual Eigen::VectorXd filt(const Eigen::VectorXd& x, 
                                double dt = -1.0,
                                double y0 = 0.0);

    // String representation
    std::string toString() const override;
};

/**
 * General linear time-invariant (LTI) system synapse.
 * 
 * Equivalent to nengo.synapses.LinearFilter in Python version.
 * 
 * This class can be used to implement any linear filter, given the
 * filter's transfer function.
 * 
 * Parameters
 * ----------
 * num : vector<double>
 *     Numerator coefficients of transfer function.
 * den : vector<double>
 *     Denominator coefficients of transfer function.
 * analog : bool, optional
 *     Whether the synapse coefficients are analog (i.e. continuous-time),
 *     or discrete. Analog coefficients will be converted to discrete for
 *     simulation using the simulator dt (default: true).
 * 
 * Attributes
 * ----------
 * analog : bool
 *     Whether the synapse coefficients are analog (i.e. continuous-time),
 *     or discrete.
 * den : vector<double>
 *     Denominator coefficients of transfer function.
 * num : vector<double>
 *     Numerator coefficients of transfer function.
 */
class LinearFilter : public Synapse {
public:
    explicit LinearFilter(const std::vector<double>& num,
                         const std::vector<double>& den,
                         bool analog = true,
                         int default_size_in = 1,
                         int default_size_out = -1,
                         double default_dt = 0.001,
                         std::optional<int> seed = std::nullopt);
    virtual ~LinearFilter() = default;

    // Getters
    const std::vector<double>& getNum() const { return num_; }
    const std::vector<double>& getDen() const { return den_; }
    bool isAnalog() const { return analog_; }

    // Setters
    void setNum(const std::vector<double>& num) { num_ = num; }
    void setDen(const std::vector<double>& den) { den_ = den; }
    void setAnalog(bool analog) { analog_ = analog; }

    std::shared_ptr<void> makeState(const std::vector<int>& shape_in,
                                   const std::vector<int>& shape_out,
                                   double dt) override;

    std::function<Eigen::VectorXd(double, const Eigen::VectorXd&)> 
    makeStep(const std::vector<int>& shape_in,
             const std::vector<int>& shape_out,
             double dt,
             std::shared_ptr<void> state) override;

    std::string toString() const override;

private:
    std::vector<double> num_;
    std::vector<double> den_;
    bool analog_;

    // Convert continuous-time to discrete-time filter
    void discretize(double dt, std::vector<double>& num_d, std::vector<double>& den_d) const;
};

/**
 * Standard first-order lowpass filter synapse.
 * 
 * Equivalent to nengo.synapses.Lowpass in Python version.
 * 
 * The impulse-response function is given by:
 *     f(t) = (1 / tau) * exp(-t / tau)
 * 
 * Parameters
 * ----------
 * tau : double
 *     The time constant of the filter in seconds.
 * 
 * Attributes
 * ----------
 * tau : double
 *     The time constant of the filter in seconds.
 */
class Lowpass : public LinearFilter {
public:
    explicit Lowpass(double tau,
                    int default_size_in = 1,
                    int default_size_out = -1,
                    double default_dt = 0.001,
                    std::optional<int> seed = std::nullopt);
    virtual ~Lowpass() = default;

    // Getter
    double getTau() const { return tau_; }

    // Setter
    void setTau(double tau);

    std::string toString() const override;

private:
    double tau_;
};

/**
 * Alpha-function filter synapse.
 * 
 * Equivalent to nengo.synapses.Alpha in Python version.
 * 
 * The impulse-response function is given by:
 *     alpha(t) = (t / tau**2) * exp(-t / tau)
 * 
 * and was found to be a good basic model for synapses.
 * 
 * Parameters
 * ----------
 * tau : double
 *     The time constant of the filter in seconds.
 * 
 * Attributes
 * ----------
 * tau : double
 *     The time constant of the filter in seconds.
 */
class Alpha : public LinearFilter {
public:
    explicit Alpha(double tau,
                  int default_size_in = 1,
                  int default_size_out = -1,
                  double default_dt = 0.001,
                  std::optional<int> seed = std::nullopt);
    virtual ~Alpha() = default;

    // Getter
    double getTau() const { return tau_; }

    // Setter
    void setTau(double tau);

    std::string toString() const override;

private:
    double tau_;
};

/**
 * Triangular finite impulse response (FIR) synapse.
 * 
 * Equivalent to nengo.synapses.Triangle in Python version.
 * 
 * This synapse has a triangular and finite impulse response. The length of
 * the triangle is t seconds; thus the digital filter will have
 * t / dt + 1 taps.
 * 
 * Parameters
 * ----------
 * t : double
 *     Length of the triangle, in seconds.
 * 
 * Attributes
 * ----------
 * t : double
 *     Length of the triangle, in seconds.
 */
class Triangle : public Synapse {
public:
    explicit Triangle(double t,
                     int default_size_in = 1,
                     int default_size_out = -1,
                     double default_dt = 0.001,
                     std::optional<int> seed = std::nullopt);
    virtual ~Triangle() = default;

    // Getter
    double getT() const { return t_; }

    // Setter
    void setT(double t) { t_ = t; }

    std::shared_ptr<void> makeState(const std::vector<int>& shape_in,
                                   const std::vector<int>& shape_out,
                                   double dt) override;

    std::function<Eigen::VectorXd(double, const Eigen::VectorXd&)> 
    makeStep(const std::vector<int>& shape_in,
             const std::vector<int>& shape_out,
             double dt,
             std::shared_ptr<void> state) override;

    std::string toString() const override;

    // Coefficient structure for filter implementation
    struct Coefficients {
        int n_taps;
        double n0;
        double ndiff;
    };

private:
    double t_;

    // Get filter coefficients for given dt
    Coefficients getCoefficients(double dt) const;
};

} // namespace nengo