#pragma once

#include <Eigen/Dense>
#include <random>
#include <memory>
#include <functional>

namespace nengo {

/**
 * Base class for probability distributions.
 * 
 * The only thing that a probability distribution needs to define is a
 * sample method. This base class ensures that all distributions
 * accept the same arguments for the sample function.
 */
class Distribution {
public:
    virtual ~Distribution() = default;
    
    /**
     * Sample from the distribution.
     * 
     * Parameters
     * ----------
     * n : int
     *     Number of samples to take.
     * d : int, optional
     *     Number of dimensions. If 0, returns vector of size n.
     *     If > 0, returns matrix of size (n, d).
     * 
     * Returns
     * -------
     * Eigen::MatrixXd
     *     Samples from the distribution.
     */
    virtual Eigen::MatrixXd sample(int n, int d = 0) = 0;
    
    /**
     * Get a single sample.
     * 
     * Returns
     * -------
     * double
     *     Single sample from the distribution.
     */
    virtual double sampleScalar() = 0;
};

/**
 * Gaussian (normal) distribution.
 */
class Gaussian : public Distribution {
public:
    /**
     * Constructor for Gaussian distribution.
     * 
     * Parameters
     * ----------
     * mean : double, optional
     *     Mean of the distribution. Default is 0.
     * std : double, optional
     *     Standard deviation. Default is 1.
     * seed : int, optional
     *     Random seed. Default is random.
     */
    explicit Gaussian(double mean = 0.0, double std = 1.0, 
                     std::optional<int> seed = std::nullopt);
    
    Eigen::MatrixXd sample(int n, int d = 0) override;
    double sampleScalar() override;
    
    // Getters
    double getMean() const { return mean_; }
    double getStd() const { return std_; }

private:
    double mean_;
    double std_;
    std::mt19937 rng_;
    std::normal_distribution<double> dist_;
};

/**
 * Uniform distribution.
 */
class Uniform : public Distribution {
public:
    /**
     * Constructor for Uniform distribution.
     * 
     * Parameters
     * ----------
     * low : double, optional
     *     Lower bound. Default is -1.
     * high : double, optional
     *     Upper bound. Default is 1.
     * seed : int, optional
     *     Random seed. Default is random.
     */
    explicit Uniform(double low = -1.0, double high = 1.0,
                    std::optional<int> seed = std::nullopt);
    
    Eigen::MatrixXd sample(int n, int d = 0) override;
    double sampleScalar() override;
    
    // Getters
    double getLow() const { return low_; }
    double getHigh() const { return high_; }

private:
    double low_;
    double high_;
    std::mt19937 rng_;
    std::uniform_real_distribution<double> dist_;
};

/**
 * Choice distribution for discrete values.
 */
class Choice : public Distribution {
public:
    /**
     * Constructor for Choice distribution.
     * 
     * Parameters
     * ----------
     * choices : std::vector<double>
     *     Available choices.
     * weights : std::vector<double>, optional
     *     Weights for each choice. Default is uniform.
     * seed : int, optional
     *     Random seed. Default is random.
     */
    explicit Choice(const std::vector<double>& choices,
                   const std::vector<double>& weights = {},
                   std::optional<int> seed = std::nullopt);
    
    Eigen::MatrixXd sample(int n, int d = 0) override;
    double sampleScalar() override;
    
    // Getters
    const std::vector<double>& getChoices() const { return choices_; }
    const std::vector<double>& getWeights() const { return weights_; }

private:
    std::vector<double> choices_;
    std::vector<double> weights_;
    std::mt19937 rng_;
    std::discrete_distribution<int> dist_;
};

/**
 * Exponential distribution.
 */
class Exponential : public Distribution {
public:
    /**
     * Constructor for Exponential distribution.
     * 
     * Parameters
     * ----------
     * scale : double, optional
     *     Scale parameter. Default is 1.
     * shift : double, optional
     *     Shift parameter. Default is 0.
     * high : double, optional
     *     Upper bound for truncation. Default is inf.
     * seed : int, optional
     *     Random seed. Default is random.
     */
    explicit Exponential(double scale = 1.0, double shift = 0.0,
                        double high = std::numeric_limits<double>::infinity(),
                        std::optional<int> seed = std::nullopt);
    
    Eigen::MatrixXd sample(int n, int d = 0) override;
    double sampleScalar() override;
    
    // Getters
    double getScale() const { return scale_; }
    double getShift() const { return shift_; }
    double getHigh() const { return high_; }

private:
    double scale_;
    double shift_;
    double high_;
    std::mt19937 rng_;
    std::exponential_distribution<double> dist_;
};

/**
 * Distribution on the surface of a unit hypersphere.
 */
class UniformHypersphere : public Distribution {
public:
    /**
     * Constructor for UniformHypersphere distribution.
     * 
     * Parameters
     * ----------
     * surface : bool, optional
     *     Whether to sample on the surface (true) or in the volume (false).
     * seed : int, optional
     *     Random seed. Default is random.
     */
    explicit UniformHypersphere(bool surface = true,
                               std::optional<int> seed = std::nullopt);
    
    Eigen::MatrixXd sample(int n, int d = 0) override;
    double sampleScalar() override;
    
    // Getters
    bool isSurface() const { return surface_; }

private:
    bool surface_;
    std::mt19937 rng_;
    std::normal_distribution<double> normal_dist_;
    std::uniform_real_distribution<double> uniform_dist_;
};

/**
 * Convenience function to get samples from a distribution or return samples.
 * 
 * Parameters
 * ----------
 * dist_or_samples : std::shared_ptr<Distribution> or Eigen::MatrixXd
 *     Either a distribution to sample from or pre-computed samples.
 * n : int
 *     Number of samples needed.
 * d : int, optional
 *     Number of dimensions.
 * 
 * Returns
 * -------
 * Eigen::MatrixXd
 *     Samples.
 */
Eigen::MatrixXd getSamples(std::shared_ptr<Distribution> dist, int n, int d = 0);
Eigen::MatrixXd getSamples(const Eigen::MatrixXd& samples, int n, int d = 0);

} // namespace nengo