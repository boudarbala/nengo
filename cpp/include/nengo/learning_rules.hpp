#pragma once

#include "nengo/synapses.hpp"
#include <Eigen/Dense>
#include <memory>

namespace nengo {

/**
 * Base class for all learning rule objects.
 * 
 * Learning rules modify connection weights based on pre and post synaptic activity.
 */
class LearningRuleType {
public:
    virtual ~LearningRuleType() = default;
    
    /**
     * Apply learning rule to update weights.
     * 
     * Parameters
     * ----------
     * weights : Eigen::MatrixXd&
     *     Current weight matrix to update.
     * pre_activities : Eigen::VectorXd
     *     Pre-synaptic activities.
     * post_activities : Eigen::VectorXd
     *     Post-synaptic activities.
     * dt : double
     *     Time step.
     */
    virtual void apply(Eigen::MatrixXd& weights,
                      const Eigen::VectorXd& pre_activities,
                      const Eigen::VectorXd& post_activities,
                      double dt) = 0;
    
    /**
     * Get the learning rate.
     */
    virtual double getLearningRate() const = 0;
    
    /**
     * Set the learning rate.
     */
    virtual void setLearningRate(double rate) = 0;
};

/**
 * Prescribed Error Sensitivity (PES) learning rule.
 * 
 * The PES learning rule adjusts connection weights based on an error signal
 * to improve the representation of a desired function.
 */
class PES : public LearningRuleType {
public:
    /**
     * Constructor for PES learning rule.
     * 
     * Parameters
     * ----------
     * learning_rate : double, optional
     *     Learning rate coefficient. Default is 1e-4.
     * pre_synapse : std::shared_ptr<Synapse>, optional
     *     Synapse for filtering pre-synaptic activities. Default is nullptr.
     */
    explicit PES(double learning_rate = 1e-4, 
                std::shared_ptr<Synapse> pre_synapse = nullptr);
    
    void apply(Eigen::MatrixXd& weights,
              const Eigen::VectorXd& pre_activities,
              const Eigen::VectorXd& post_activities,
              double dt) override;
    
    // Set error signal for PES learning
    void setError(const Eigen::VectorXd& error) { error_ = error; }
    
    double getLearningRate() const override { return learning_rate_; }
    void setLearningRate(double rate) override { learning_rate_ = rate; }
    
    std::shared_ptr<Synapse> getPreSynapse() const { return pre_synapse_; }

private:
    double learning_rate_;
    std::shared_ptr<Synapse> pre_synapse_;
    Eigen::VectorXd error_;
    Eigen::VectorXd filtered_pre_;
    bool synapse_initialized_;
};

/**
 * Bienenstock-Cooper-Munro (BCM) learning rule.
 * 
 * The BCM rule is a sliding threshold learning rule that modifies synaptic
 * strengths based on the correlation between pre and post activities.
 */
class BCM : public LearningRuleType {
public:
    /**
     * Constructor for BCM learning rule.
     * 
     * Parameters
     * ----------
     * learning_rate : double, optional
     *     Learning rate coefficient. Default is 1e-9.
     * theta_tau : double, optional
     *     Time constant for sliding threshold. Default is 1.0.
     * pre_synapse : std::shared_ptr<Synapse>, optional
     *     Synapse for filtering pre-synaptic activities. Default is nullptr.
     * post_synapse : std::shared_ptr<Synapse>, optional
     *     Synapse for filtering post-synaptic activities. Default is nullptr.
     */
    explicit BCM(double learning_rate = 1e-9, double theta_tau = 1.0,
                std::shared_ptr<Synapse> pre_synapse = nullptr,
                std::shared_ptr<Synapse> post_synapse = nullptr);
    
    void apply(Eigen::MatrixXd& weights,
              const Eigen::VectorXd& pre_activities,
              const Eigen::VectorXd& post_activities,
              double dt) override;
    
    double getLearningRate() const override { return learning_rate_; }
    void setLearningRate(double rate) override { learning_rate_ = rate; }
    
    double getThetaTau() const { return theta_tau_; }
    void setThetaTau(double tau) { theta_tau_ = tau; }

private:
    double learning_rate_;
    double theta_tau_;
    std::shared_ptr<Synapse> pre_synapse_;
    std::shared_ptr<Synapse> post_synapse_;
    
    Eigen::VectorXd theta_;  // Sliding threshold
    Eigen::VectorXd filtered_pre_;
    Eigen::VectorXd filtered_post_;
    bool initialized_;
};

/**
 * Oja's learning rule.
 * 
 * A normalized Hebbian learning rule that prevents unlimited weight growth.
 */
class Oja : public LearningRuleType {
public:
    /**
     * Constructor for Oja learning rule.
     * 
     * Parameters
     * ----------
     * learning_rate : double, optional
     *     Learning rate coefficient. Default is 1e-6.
     * pre_synapse : std::shared_ptr<Synapse>, optional
     *     Synapse for filtering pre-synaptic activities. Default is nullptr.
     * post_synapse : std::shared_ptr<Synapse>, optional
     *     Synapse for filtering post-synaptic activities. Default is nullptr.
     */
    explicit Oja(double learning_rate = 1e-6,
                std::shared_ptr<Synapse> pre_synapse = nullptr,
                std::shared_ptr<Synapse> post_synapse = nullptr);
    
    void apply(Eigen::MatrixXd& weights,
              const Eigen::VectorXd& pre_activities,
              const Eigen::VectorXd& post_activities,
              double dt) override;
    
    double getLearningRate() const override { return learning_rate_; }
    void setLearningRate(double rate) override { learning_rate_ = rate; }

private:
    double learning_rate_;
    std::shared_ptr<Synapse> pre_synapse_;
    std::shared_ptr<Synapse> post_synapse_;
    
    Eigen::VectorXd filtered_pre_;
    Eigen::VectorXd filtered_post_;
    bool initialized_;
};

/**
 * Voja (Vector Oja) learning rule.
 * 
 * A learning rule that modifies encoders rather than decoders.
 */
class Voja : public LearningRuleType {
public:
    /**
     * Constructor for Voja learning rule.
     * 
     * Parameters
     * ----------
     * learning_rate : double, optional
     *     Learning rate coefficient. Default is 1e-2.
     * post_synapse : std::shared_ptr<Synapse>, optional
     *     Synapse for filtering post-synaptic activities. Default is nullptr.
     */
    explicit Voja(double learning_rate = 1e-2,
                 std::shared_ptr<Synapse> post_synapse = nullptr);
    
    void apply(Eigen::MatrixXd& weights,
              const Eigen::VectorXd& pre_activities,
              const Eigen::VectorXd& post_activities,
              double dt) override;
    
    double getLearningRate() const override { return learning_rate_; }
    void setLearningRate(double rate) override { learning_rate_ = rate; }

private:
    double learning_rate_;
    std::shared_ptr<Synapse> post_synapse_;
    
    Eigen::VectorXd filtered_post_;
    bool initialized_;
};

} // namespace nengo