#pragma once

#include <Eigen/Dense>
#include <memory>

namespace nengo {

/**
 * Base class for decoder or weight solvers.
 * 
 * A solver can have the weights parameter equal to true or false.
 * Weight solvers are used to form neuron-to-neuron weight matrices.
 */
class Solver {
public:
    virtual ~Solver() = default;
    
    /**
     * Solve for weights given activities and targets.
     * 
     * Parameters
     * ----------
     * activities : Eigen::MatrixXd
     *     Matrix of neuron activities (samples x neurons).
     * targets : Eigen::MatrixXd
     *     Matrix of target values (samples x dimensions).
     * 
     * Returns
     * -------
     * Eigen::MatrixXd
     *     Solved weights matrix (neurons x dimensions).
     */
    virtual Eigen::MatrixXd solve(const Eigen::MatrixXd& activities, 
                                 const Eigen::MatrixXd& targets) const = 0;
    
    // Whether this solver is compositional
    virtual bool isCompositional() const { return true; }
    
    // Whether this solver produces weights (vs decoders)
    virtual bool isWeights() const { return false; }
};

/**
 * Least-squares solver.
 * 
 * This is the default solver that uses least-squares optimization
 * to find optimal decoders or weights.
 */
class LstsqL2 : public Solver {
public:
    /**
     * Constructor for LstsqL2 solver.
     * 
     * Parameters
     * ----------
     * reg : double, optional
     *     Regularization parameter. Default is 0.1.
     * weights : bool, optional
     *     Whether to solve for weights. Default is false (solve for decoders).
     */
    explicit LstsqL2(double reg = 0.1, bool weights = false);
    
    Eigen::MatrixXd solve(const Eigen::MatrixXd& activities, 
                         const Eigen::MatrixXd& targets) const override;
    
    bool isWeights() const override { return weights_; }
    
    // Getters
    double getRegularization() const { return reg_; }

private:
    double reg_;
    bool weights_;
};

/**
 * L1-regularized least-squares solver.
 * 
 * Uses L1 regularization (LASSO) for sparse solutions.
 */
class LstsqL1 : public Solver {
public:
    /**
     * Constructor for LstsqL1 solver.
     * 
     * Parameters
     * ----------
     * reg : double, optional
     *     L1 regularization parameter. Default is 0.1.
     * weights : bool, optional
     *     Whether to solve for weights. Default is false.
     */
    explicit LstsqL1(double reg = 0.1, bool weights = false);
    
    Eigen::MatrixXd solve(const Eigen::MatrixXd& activities, 
                         const Eigen::MatrixXd& targets) const override;
    
    bool isWeights() const override { return weights_; }
    
    // Getters
    double getRegularization() const { return reg_; }

private:
    double reg_;
    bool weights_;
    
    // Simplified L1 solver using iterative soft thresholding
    Eigen::MatrixXd solveL1(const Eigen::MatrixXd& A, 
                           const Eigen::MatrixXd& Y) const;
};

/**
 * Non-negative least-squares solver.
 * 
 * Constrains all decoder values to be non-negative.
 */
class Nnls : public Solver {
public:
    /**
     * Constructor for Nnls solver.
     * 
     * Parameters
     * ----------
     * reg : double, optional
     *     Regularization parameter. Default is 0.1.
     * weights : bool, optional
     *     Whether to solve for weights. Default is false.
     */
    explicit Nnls(double reg = 0.1, bool weights = false);
    
    Eigen::MatrixXd solve(const Eigen::MatrixXd& activities, 
                         const Eigen::MatrixXd& targets) const override;
    
    bool isWeights() const override { return weights_; }
    
    // Getters
    double getRegularization() const { return reg_; }

private:
    double reg_;
    bool weights_;
    
    // Simplified NNLS using projected gradient descent
    Eigen::VectorXd solveNnls(const Eigen::MatrixXd& A, 
                             const Eigen::VectorXd& b) const;
};

/**
 * Solver that returns zeros.
 * 
 * Useful for testing or when no connection is desired.
 */
class NoSolver : public Solver {
public:
    explicit NoSolver(bool weights = false) : weights_(weights) {}
    
    Eigen::MatrixXd solve(const Eigen::MatrixXd& activities, 
                         const Eigen::MatrixXd& targets) const override;
    
    bool isWeights() const override { return weights_; }

private:
    bool weights_;
};

} // namespace nengo