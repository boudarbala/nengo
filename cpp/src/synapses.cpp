#include "nengo/synapses.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <sstream>

namespace nengo {

// Synapse base class implementation

Synapse::Synapse(int default_size_in, int default_size_out, double default_dt, std::optional<int> seed)
    : Process(default_size_in, 
              default_size_out == -1 ? default_size_in : default_size_out, 
              default_dt, seed) {
}

Eigen::VectorXd Synapse::filt(const Eigen::VectorXd& x, double dt, double y0) {
    if (dt < 0) {
        dt = getDefaultDt();
    }
    
    std::vector<int> shape_in = {1}; // Single signal processing
    std::vector<int> shape_out = {1};
    
    auto state = makeState(shape_in, shape_out, dt);
    auto step = makeStep(shape_in, shape_out, dt, state);
    
    Eigen::VectorXd filtered(x.size());
    
    for (int i = 0; i < x.size(); ++i) {
        Eigen::VectorXd signal_in(1);
        signal_in(0) = x(i);
        Eigen::VectorXd result = step(i * dt, signal_in);
        filtered(i) = result(0);
    }
    
    return filtered;
}

std::string Synapse::toString() const {
    return "Synapse()";
}

// LinearFilter implementation

LinearFilter::LinearFilter(const std::vector<double>& num, const std::vector<double>& den,
                          bool analog, int default_size_in, int default_size_out,
                          double default_dt, std::optional<int> seed)
    : Synapse(default_size_in, default_size_out, default_dt, seed),
      num_(num), den_(den), analog_(analog) {
}

void LinearFilter::discretize(double dt, std::vector<double>& num_d, std::vector<double>& den_d) const {
    if (!analog_) {
        num_d = num_;
        den_d = den_;
        return;
    }
    
    // Simple Tustin transformation (bilinear transform)
    // For first-order systems: s -> 2/dt * (z-1)/(z+1)
    
    if (den_.size() == 2 && num_.size() <= 2) {
        // First-order case: H(s) = num[0] / (den[0]*s + den[1])
        double a = den_[0];
        double b = den_[1];
        double K = num_.size() > 0 ? num_[0] : 1.0;
        
        // Bilinear transform
        double alpha = 2.0 / dt;
        den_d = {a * alpha + b, -a * alpha + b};
        num_d = {K, K};
        
        // Normalize
        double norm = den_d[0];
        for (auto& d : den_d) d /= norm;
        for (auto& n : num_d) n /= norm;
    } else if (den_.size() == 3 && num_.size() <= 3) {
        // Second-order case: H(s) = num[0] / (den[0]*s^2 + den[1]*s + den[2])
        double a = den_[0];
        double b = den_[1]; 
        double c = den_[2];
        double K = num_.size() > 0 ? num_[0] : 1.0;
        
        // Bilinear transform for second-order
        double alpha = 2.0 / dt;
        double alpha2 = alpha * alpha;
        
        den_d = {
            a * alpha2 + b * alpha + c,
            -2 * a * alpha2 + 2 * c,
            a * alpha2 - b * alpha + c
        };
        num_d = {K, 2*K, K};
        
        // Normalize
        double norm = den_d[0];
        for (auto& d : den_d) d /= norm;
        for (auto& n : num_d) n /= norm;
    } else {
        // For higher-order systems, use simple approximation
        // Just copy and assume they're already discrete
        num_d = num_;
        den_d = den_;
    }
}

// State structure for LinearFilter
struct LinearFilterState {
    Eigen::VectorXd x;  // State vector
    Eigen::MatrixXd A;  // State matrix
    Eigen::VectorXd B;  // Input matrix
    Eigen::VectorXd C;  // Output matrix  
    double D;           // Feedthrough matrix
    std::vector<int> shape_out;
};

std::shared_ptr<void> LinearFilter::makeState(const std::vector<int>& shape_in,
                                             const std::vector<int>& shape_out,
                                             double dt) {
    auto state = std::make_shared<LinearFilterState>();
    
    std::vector<double> num_d, den_d;
    discretize(dt, num_d, den_d);
    
    state->shape_out = shape_out;
    
    // Convert to state-space representation
    int n = static_cast<int>(den_d.size()) - 1; // Order of the system
    
    if (n == 0) {
        // Direct feedthrough only
        state->A = Eigen::MatrixXd::Zero(0, 0);
        state->B = Eigen::VectorXd::Zero(0);
        state->C = Eigen::VectorXd::Zero(0);
        state->D = num_d[0] / den_d[0];
        state->x = Eigen::VectorXd::Zero(0);
    } else {
        // Create companion form state-space matrices
        state->A = Eigen::MatrixXd::Zero(n, n);
        state->B = Eigen::VectorXd::Zero(n);
        state->C = Eigen::VectorXd::Zero(n);
        
        // Companion form
        for (int i = 0; i < n - 1; ++i) {
            state->A(i, i + 1) = 1.0;
        }
        for (int i = 0; i < n; ++i) {
            state->A(n - 1, i) = -den_d[n - i] / den_d[0];
        }
        
        state->B(n - 1) = 1.0 / den_d[0];
        
        // Output matrix
        for (int i = 0; i < std::min(n, static_cast<int>(num_d.size())); ++i) {
            state->C(i) = num_d[num_d.size() - 1 - i];
        }
        
        state->D = (num_d.size() > n) ? num_d[0] / den_d[0] : 0.0;
        
        // Initialize state
        state->x = Eigen::VectorXd::Zero(n);
    }
    
    return state;
}

std::function<Eigen::VectorXd(double, const Eigen::VectorXd&)> 
LinearFilter::makeStep(const std::vector<int>& shape_in,
                       const std::vector<int>& shape_out,
                       double dt,
                       std::shared_ptr<void> state) {
    auto filter_state = std::static_pointer_cast<LinearFilterState>(state);
    
    return [filter_state](double t, const Eigen::VectorXd& signal) -> Eigen::VectorXd {
        if (filter_state->x.size() == 0) {
            // Direct feedthrough only
            return filter_state->D * signal;
        }
        
        // Linear system step: x[k+1] = A*x[k] + B*u[k], y[k] = C*x[k] + D*u[k]
        Eigen::VectorXd y(signal.size());
        
        // Compute output for each signal element
        for (int i = 0; i < signal.size(); ++i) {
            double u = signal(i);
            double output = filter_state->C.dot(filter_state->x) + filter_state->D * u;
            y(i) = output;
            
            // Update state
            filter_state->x = filter_state->A * filter_state->x + filter_state->B * u;
        }
        
        return y;
    };
}

std::string LinearFilter::toString() const {
    std::stringstream ss;
    ss << "LinearFilter(num=[";
    for (size_t i = 0; i < num_.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << num_[i];
    }
    ss << "], den=[";
    for (size_t i = 0; i < den_.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << den_[i];
    }
    ss << "], analog=" << (analog_ ? "true" : "false") << ")";
    return ss.str();
}

// Lowpass implementation

Lowpass::Lowpass(double tau, int default_size_in, int default_size_out,
                 double default_dt, std::optional<int> seed)
    : LinearFilter({1.0}, {tau, 1.0}, true, default_size_in, default_size_out, default_dt, seed),
      tau_(tau) {
}

void Lowpass::setTau(double tau) {
    tau_ = tau;
    setDen({tau, 1.0});
}

std::string Lowpass::toString() const {
    std::stringstream ss;
    ss << "Lowpass(tau=" << tau_ << ")";
    return ss.str();
}

// Alpha implementation

Alpha::Alpha(double tau, int default_size_in, int default_size_out,
             double default_dt, std::optional<int> seed)
    : LinearFilter({1.0}, {tau * tau, 2.0 * tau, 1.0}, true, 
                   default_size_in, default_size_out, default_dt, seed),
      tau_(tau) {
}

void Alpha::setTau(double tau) {
    tau_ = tau;
    setDen({tau * tau, 2.0 * tau, 1.0});
}

std::string Alpha::toString() const {
    std::stringstream ss;
    ss << "Alpha(tau=" << tau_ << ")";
    return ss.str();
}

// Triangle implementation

Triangle::Triangle(double t, int default_size_in, int default_size_out,
                   double default_dt, std::optional<int> seed)
    : Synapse(default_size_in, default_size_out, default_dt, seed), t_(t) {
}

Triangle::Coefficients Triangle::getCoefficients(double dt) const {
    Coefficients coeff;
    coeff.n_taps = static_cast<int>(std::round(t_ / dt)) + 1;
    
    // Create triangular coefficients
    std::vector<double> num(coeff.n_taps);
    for (int i = 0; i < coeff.n_taps; ++i) {
        num[i] = static_cast<double>(coeff.n_taps - i);
    }
    
    // Normalize
    double sum = 0.0;
    for (double val : num) sum += val;
    for (double& val : num) val /= sum;
    
    coeff.n0 = num[0];
    coeff.ndiff = num[coeff.n_taps - 1];
    
    return coeff;
}

// State structure for Triangle
struct TriangleState {
    Eigen::VectorXd Y;   // Output buffer
    Eigen::MatrixXd X;   // Circular buffer for input history
    int Xi;              // Current position in circular buffer
    std::vector<int> shape_out;
    Triangle::Coefficients coeff;
};

std::shared_ptr<void> Triangle::makeState(const std::vector<int>& shape_in,
                                         const std::vector<int>& shape_out,
                                         double dt) {
    auto state = std::make_shared<TriangleState>();
    state->shape_out = shape_out;
    state->coeff = getCoefficients(dt);
    
    // Initialize buffers
    int signal_size = 1;
    if (!shape_out.empty()) {
        signal_size = shape_out[0];
    }
    
    state->Y = Eigen::VectorXd::Zero(signal_size);
    state->X = Eigen::MatrixXd::Zero(state->coeff.n_taps, signal_size);
    state->Xi = 0;
    
    return state;
}

std::function<Eigen::VectorXd(double, const Eigen::VectorXd&)> 
Triangle::makeStep(const std::vector<int>& shape_in,
                   const std::vector<int>& shape_out,
                   double dt,
                   std::shared_ptr<void> state) {
    auto tri_state = std::static_pointer_cast<TriangleState>(state);
    
    return [tri_state](double t, const Eigen::VectorXd& signal) -> Eigen::VectorXd {
        // Triangle filter implementation
        Eigen::VectorXd& Y = tri_state->Y;
        Eigen::MatrixXd& X = tri_state->X;
        int& Xi = tri_state->Xi;
        const auto& coeff = tri_state->coeff;
        
        // Ensure signal is the right size
        Eigen::VectorXd input_signal = signal;
        if (input_signal.size() != Y.size()) {
            input_signal = Eigen::VectorXd::Constant(Y.size(), signal(0));
        }
        
        // Add new input scaled by first coefficient
        Y += coeff.n0 * input_signal;
        
        // Subtract sum of all stored values
        for (int i = 0; i < X.rows(); ++i) {
            Y -= X.row(i).transpose();
        }
        
        // Update circular buffer
        Xi = (Xi + 1) % coeff.n_taps;
        if (coeff.n_taps > 0) {
            X.row(Xi) = (coeff.ndiff * input_signal).transpose();
        }
        
        return Y;
    };
}

std::string Triangle::toString() const {
    std::stringstream ss;
    ss << "Triangle(t=" << t_ << ")";
    return ss.str();
}

} // namespace nengo