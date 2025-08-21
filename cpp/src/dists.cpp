#include "nengo/dists.hpp"
#include <cmath>
#include <stdexcept>

namespace nengo {

// Gaussian Implementation

Gaussian::Gaussian(double mean, double std, std::optional<int> seed)
    : mean_(mean), std_(std) {
    
    if (std <= 0) {
        throw std::invalid_argument("Standard deviation must be positive");
    }
    
    if (seed.has_value()) {
        rng_.seed(seed.value());
    } else {
        std::random_device rd;
        rng_.seed(rd());
    }
    
    dist_ = std::normal_distribution<double>(mean_, std_);
}

Eigen::MatrixXd Gaussian::sample(int n, int d) {
    if (n <= 0) {
        throw std::invalid_argument("Number of samples must be positive");
    }
    
    if (d == 0) {
        Eigen::MatrixXd samples(n, 1);
        for (int i = 0; i < n; ++i) {
            samples(i, 0) = dist_(rng_);
        }
        return samples;
    } else {
        Eigen::MatrixXd samples(n, d);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < d; ++j) {
                samples(i, j) = dist_(rng_);
            }
        }
        return samples;
    }
}

double Gaussian::sampleScalar() {
    return dist_(rng_);
}

// Uniform Implementation

Uniform::Uniform(double low, double high, std::optional<int> seed)
    : low_(low), high_(high) {
    
    if (low >= high) {
        throw std::invalid_argument("Low bound must be less than high bound");
    }
    
    if (seed.has_value()) {
        rng_.seed(seed.value());
    } else {
        std::random_device rd;
        rng_.seed(rd());
    }
    
    dist_ = std::uniform_real_distribution<double>(low_, high_);
}

Eigen::MatrixXd Uniform::sample(int n, int d) {
    if (n <= 0) {
        throw std::invalid_argument("Number of samples must be positive");
    }
    
    if (d == 0) {
        Eigen::MatrixXd samples(n, 1);
        for (int i = 0; i < n; ++i) {
            samples(i, 0) = dist_(rng_);
        }
        return samples;
    } else {
        Eigen::MatrixXd samples(n, d);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < d; ++j) {
                samples(i, j) = dist_(rng_);
            }
        }
        return samples;
    }
}

double Uniform::sampleScalar() {
    return dist_(rng_);
}

// Choice Implementation

Choice::Choice(const std::vector<double>& choices,
               const std::vector<double>& weights,
               std::optional<int> seed)
    : choices_(choices) {
    
    if (choices.empty()) {
        throw std::invalid_argument("Choices cannot be empty");
    }
    
    if (weights.empty()) {
        // Uniform weights
        weights_ = std::vector<double>(choices.size(), 1.0);
    } else {
        if (weights.size() != choices.size()) {
            throw std::invalid_argument("Weights and choices must have same size");
        }
        weights_ = weights;
    }
    
    if (seed.has_value()) {
        rng_.seed(seed.value());
    } else {
        std::random_device rd;
        rng_.seed(rd());
    }
    
    dist_ = std::discrete_distribution<int>(weights_.begin(), weights_.end());
}

Eigen::MatrixXd Choice::sample(int n, int d) {
    if (n <= 0) {
        throw std::invalid_argument("Number of samples must be positive");
    }
    
    if (d == 0) {
        Eigen::MatrixXd samples(n, 1);
        for (int i = 0; i < n; ++i) {
            int idx = dist_(rng_);
            samples(i, 0) = choices_[idx];
        }
        return samples;
    } else {
        Eigen::MatrixXd samples(n, d);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < d; ++j) {
                int idx = dist_(rng_);
                samples(i, j) = choices_[idx];
            }
        }
        return samples;
    }
}

double Choice::sampleScalar() {
    int idx = dist_(rng_);
    return choices_[idx];
}

// Exponential Implementation

Exponential::Exponential(double scale, double shift, double high, std::optional<int> seed)
    : scale_(scale), shift_(shift), high_(high) {
    
    if (scale <= 0) {
        throw std::invalid_argument("Scale must be positive");
    }
    
    if (seed.has_value()) {
        rng_.seed(seed.value());
    } else {
        std::random_device rd;
        rng_.seed(rd());
    }
    
    dist_ = std::exponential_distribution<double>(1.0 / scale_);
}

Eigen::MatrixXd Exponential::sample(int n, int d) {
    if (n <= 0) {
        throw std::invalid_argument("Number of samples must be positive");
    }
    
    if (d == 0) {
        Eigen::MatrixXd samples(n, 1);
        for (int i = 0; i < n; ++i) {
            double val;
            do {
                val = dist_(rng_) + shift_;
            } while (val > high_);
            samples(i, 0) = val;
        }
        return samples;
    } else {
        Eigen::MatrixXd samples(n, d);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < d; ++j) {
                double val;
                do {
                    val = dist_(rng_) + shift_;
                } while (val > high_);
                samples(i, j) = val;
            }
        }
        return samples;
    }
}

double Exponential::sampleScalar() {
    double val;
    do {
        val = dist_(rng_) + shift_;
    } while (val > high_);
    return val;
}

// UniformHypersphere Implementation

UniformHypersphere::UniformHypersphere(bool surface, std::optional<int> seed)
    : surface_(surface) {
    
    if (seed.has_value()) {
        rng_.seed(seed.value());
    } else {
        std::random_device rd;
        rng_.seed(rd());
    }
    
    normal_dist_ = std::normal_distribution<double>(0.0, 1.0);
    uniform_dist_ = std::uniform_real_distribution<double>(0.0, 1.0);
}

Eigen::MatrixXd UniformHypersphere::sample(int n, int d) {
    if (n <= 0) {
        throw std::invalid_argument("Number of samples must be positive");
    }
    if (d <= 0) {
        throw std::invalid_argument("Dimensions must be positive for hypersphere");
    }
    
    Eigen::MatrixXd samples(n, d);
    
    for (int i = 0; i < n; ++i) {
        // Generate Gaussian samples
        Eigen::VectorXd point(d);
        for (int j = 0; j < d; ++j) {
            point(j) = normal_dist_(rng_);
        }
        
        // Normalize to unit sphere
        double norm = point.norm();
        if (norm > 0) {
            point /= norm;
        }
        
        // For volume sampling, scale by random radius
        if (!surface_) {
            double r = std::pow(uniform_dist_(rng_), 1.0 / d);
            point *= r;
        }
        
        samples.row(i) = point.transpose();
    }
    
    return samples;
}

double UniformHypersphere::sampleScalar() {
    throw std::runtime_error("UniformHypersphere requires dimensions > 0");
}

// Utility Functions

Eigen::MatrixXd getSamples(std::shared_ptr<Distribution> dist, int n, int d) {
    if (!dist) {
        throw std::invalid_argument("Distribution cannot be null");
    }
    return dist->sample(n, d);
}

Eigen::MatrixXd getSamples(const Eigen::MatrixXd& samples, int n, int d) {
    if (samples.rows() < n) {
        throw std::invalid_argument("Not enough samples provided");
    }
    
    if (d == 0) {
        return samples.topRows(n);
    } else {
        if (samples.cols() < d) {
            throw std::invalid_argument("Not enough dimensions in samples");
        }
        return samples.topLeftCorner(n, d);
    }
}

} // namespace nengo