#include "nengo/solvers.hpp"
#include <Eigen/QR>
#include <algorithm>
#include <cmath>

namespace nengo {

// LstsqL2 Implementation

LstsqL2::LstsqL2(double reg, bool weights) : reg_(reg), weights_(weights) {
    if (reg < 0) {
        throw std::invalid_argument("Regularization parameter must be non-negative");
    }
}

Eigen::MatrixXd LstsqL2::solve(const Eigen::MatrixXd& activities, 
                              const Eigen::MatrixXd& targets) const {
    if (activities.rows() != targets.rows()) {
        throw std::invalid_argument("Activities and targets must have the same number of samples");
    }
    
    int n_neurons = activities.cols();
    int n_dims = targets.cols();
    
    // Form regularized normal equations: (A^T A + reg*I) x = A^T b
    Eigen::MatrixXd AtA = activities.transpose() * activities;
    AtA.diagonal().array() += reg_;
    
    Eigen::MatrixXd AtY = activities.transpose() * targets;
    
    // Solve using Cholesky decomposition for efficiency
    Eigen::MatrixXd decoders = AtA.ldlt().solve(AtY);
    
    return decoders;
}

// LstsqL1 Implementation

LstsqL1::LstsqL1(double reg, bool weights) : reg_(reg), weights_(weights) {
    if (reg < 0) {
        throw std::invalid_argument("Regularization parameter must be non-negative");
    }
}

Eigen::MatrixXd LstsqL1::solve(const Eigen::MatrixXd& activities, 
                              const Eigen::MatrixXd& targets) const {
    if (activities.rows() != targets.rows()) {
        throw std::invalid_argument("Activities and targets must have the same number of samples");
    }
    
    return solveL1(activities, targets);
}

Eigen::MatrixXd LstsqL1::solveL1(const Eigen::MatrixXd& A, 
                                const Eigen::MatrixXd& Y) const {
    // Simplified L1 solver using iterative soft thresholding (ISTA)
    int n_neurons = A.cols();
    int n_dims = Y.cols();
    
    Eigen::MatrixXd decoders = Eigen::MatrixXd::Zero(n_neurons, n_dims);
    
    // Compute step size based on largest eigenvalue of A^T A
    Eigen::MatrixXd AtA = A.transpose() * A;
    double L = AtA.norm();  // Approximate largest eigenvalue
    double step_size = 1.0 / (L + 1e-10);
    
    Eigen::MatrixXd AtY = A.transpose() * Y;
    
    // Iterative soft thresholding
    for (int iter = 0; iter < 100; ++iter) {  // Max iterations
        Eigen::MatrixXd gradient = AtA * decoders - AtY;
        Eigen::MatrixXd updated = decoders - step_size * gradient;
        
        // Soft thresholding
        double threshold = reg_ * step_size;
        for (int i = 0; i < updated.rows(); ++i) {
            for (int j = 0; j < updated.cols(); ++j) {
                double val = updated(i, j);
                if (val > threshold) {
                    updated(i, j) = val - threshold;
                } else if (val < -threshold) {
                    updated(i, j) = val + threshold;
                } else {
                    updated(i, j) = 0.0;
                }
            }
        }
        
        // Check convergence
        double diff = (updated - decoders).norm();
        decoders = updated;
        
        if (diff < 1e-6) {
            break;
        }
    }
    
    return decoders;
}

// Nnls Implementation

Nnls::Nnls(double reg, bool weights) : reg_(reg), weights_(weights) {
    if (reg < 0) {
        throw std::invalid_argument("Regularization parameter must be non-negative");
    }
}

Eigen::MatrixXd Nnls::solve(const Eigen::MatrixXd& activities, 
                           const Eigen::MatrixXd& targets) const {
    if (activities.rows() != targets.rows()) {
        throw std::invalid_argument("Activities and targets must have the same number of samples");
    }
    
    int n_neurons = activities.cols();
    int n_dims = targets.cols();
    
    Eigen::MatrixXd decoders(n_neurons, n_dims);
    
    // Solve NNLS for each target dimension
    for (int d = 0; d < n_dims; ++d) {
        decoders.col(d) = solveNnls(activities, targets.col(d));
    }
    
    return decoders;
}

Eigen::VectorXd Nnls::solveNnls(const Eigen::MatrixXd& A, 
                               const Eigen::VectorXd& b) const {
    // Simplified NNLS using projected gradient descent
    int n = A.cols();
    Eigen::VectorXd x = Eigen::VectorXd::Zero(n);
    
    // Form regularized system
    Eigen::MatrixXd AtA = A.transpose() * A;
    AtA.diagonal().array() += reg_;
    Eigen::VectorXd Atb = A.transpose() * b;
    
    // Compute step size
    double L = AtA.norm();
    double step_size = 1.0 / (L + 1e-10);
    
    // Projected gradient descent
    for (int iter = 0; iter < 100; ++iter) {
        Eigen::VectorXd gradient = AtA * x - Atb;
        Eigen::VectorXd updated = x - step_size * gradient;
        
        // Project onto non-negative orthant
        updated = updated.cwiseMax(0.0);
        
        // Check convergence
        double diff = (updated - x).norm();
        x = updated;
        
        if (diff < 1e-6) {
            break;
        }
    }
    
    return x;
}

// NoSolver Implementation

Eigen::MatrixXd NoSolver::solve(const Eigen::MatrixXd& activities, 
                               const Eigen::MatrixXd& targets) const {
    return Eigen::MatrixXd::Zero(activities.cols(), targets.cols());
}

} // namespace nengo