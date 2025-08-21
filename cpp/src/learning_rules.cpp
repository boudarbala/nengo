#include "nengo/learning_rules.hpp"
#include <stdexcept>
#include <cmath>

namespace nengo {

// PES Implementation

PES::PES(double learning_rate, std::shared_ptr<Synapse> pre_synapse)
    : learning_rate_(learning_rate), pre_synapse_(pre_synapse), synapse_initialized_(false) {
    
    if (learning_rate < 0) {
        throw std::invalid_argument("Learning rate must be non-negative");
    }
}

void PES::apply(Eigen::MatrixXd& weights,
               const Eigen::VectorXd& pre_activities,
               const Eigen::VectorXd& post_activities,
               double dt) {
    
    if (error_.size() == 0) {
        return;  // No error signal set
    }
    
    Eigen::VectorXd pre_filtered = pre_activities;
    
    // Filter pre-synaptic activities if synapse is provided
    if (pre_synapse_) {
        if (!synapse_initialized_) {
            filtered_pre_ = Eigen::VectorXd::Zero(pre_activities.size());
            synapse_initialized_ = true;
        }
        
        // Simple first-order filtering
        double alpha = dt / (pre_synapse_->getTau() + dt);
        filtered_pre_ = (1.0 - alpha) * filtered_pre_ + alpha * pre_activities;
        pre_filtered = filtered_pre_;
    }
    
    // PES learning rule: Δw = η * e * a_pre
    // where η is learning rate, e is error, a_pre is pre-synaptic activity
    for (int i = 0; i < weights.rows(); ++i) {
        for (int j = 0; j < weights.cols(); ++j) {
            if (j < error_.size()) {
                weights(i, j) -= learning_rate_ * error_(j) * pre_filtered(i) * dt;
            }
        }
    }
}

// BCM Implementation

BCM::BCM(double learning_rate, double theta_tau,
         std::shared_ptr<Synapse> pre_synapse,
         std::shared_ptr<Synapse> post_synapse)
    : learning_rate_(learning_rate), theta_tau_(theta_tau),
      pre_synapse_(pre_synapse), post_synapse_(post_synapse), initialized_(false) {
    
    if (learning_rate < 0) {
        throw std::invalid_argument("Learning rate must be non-negative");
    }
    if (theta_tau <= 0) {
        throw std::invalid_argument("Theta tau must be positive");
    }
}

void BCM::apply(Eigen::MatrixXd& weights,
               const Eigen::VectorXd& pre_activities,
               const Eigen::VectorXd& post_activities,
               double dt) {
    
    if (!initialized_) {
        theta_ = Eigen::VectorXd::Ones(post_activities.size());
        filtered_pre_ = Eigen::VectorXd::Zero(pre_activities.size());
        filtered_post_ = Eigen::VectorXd::Zero(post_activities.size());
        initialized_ = true;
    }
    
    Eigen::VectorXd pre_filtered = pre_activities;
    Eigen::VectorXd post_filtered = post_activities;
    
    // Filter activities if synapses are provided
    if (pre_synapse_) {
        double alpha = dt / (pre_synapse_->getTau() + dt);
        filtered_pre_ = (1.0 - alpha) * filtered_pre_ + alpha * pre_activities;
        pre_filtered = filtered_pre_;
    }
    
    if (post_synapse_) {
        double alpha = dt / (post_synapse_->getTau() + dt);
        filtered_post_ = (1.0 - alpha) * filtered_post_ + alpha * post_activities;
        post_filtered = filtered_post_;
    }
    
    // Update sliding threshold
    double theta_alpha = dt / theta_tau_;
    for (int j = 0; j < theta_.size(); ++j) {
        theta_(j) = (1.0 - theta_alpha) * theta_(j) + 
                   theta_alpha * post_filtered(j) * post_filtered(j);
    }
    
    // BCM learning rule: Δw = η * a_post * (a_post - θ) * a_pre
    for (int i = 0; i < weights.rows(); ++i) {
        for (int j = 0; j < weights.cols(); ++j) {
            if (j < post_filtered.size()) {
                double phi = post_filtered(j) * (post_filtered(j) - theta_(j));
                weights(i, j) += learning_rate_ * phi * pre_filtered(i) * dt;
            }
        }
    }
}

// Oja Implementation

Oja::Oja(double learning_rate,
         std::shared_ptr<Synapse> pre_synapse,
         std::shared_ptr<Synapse> post_synapse)
    : learning_rate_(learning_rate), pre_synapse_(pre_synapse), 
      post_synapse_(post_synapse), initialized_(false) {
    
    if (learning_rate < 0) {
        throw std::invalid_argument("Learning rate must be non-negative");
    }
}

void Oja::apply(Eigen::MatrixXd& weights,
               const Eigen::VectorXd& pre_activities,
               const Eigen::VectorXd& post_activities,
               double dt) {
    
    if (!initialized_) {
        filtered_pre_ = Eigen::VectorXd::Zero(pre_activities.size());
        filtered_post_ = Eigen::VectorXd::Zero(post_activities.size());
        initialized_ = true;
    }
    
    Eigen::VectorXd pre_filtered = pre_activities;
    Eigen::VectorXd post_filtered = post_activities;
    
    // Filter activities if synapses are provided
    if (pre_synapse_) {
        double alpha = dt / (pre_synapse_->getTau() + dt);
        filtered_pre_ = (1.0 - alpha) * filtered_pre_ + alpha * pre_activities;
        pre_filtered = filtered_pre_;
    }
    
    if (post_synapse_) {
        double alpha = dt / (post_synapse_->getTau() + dt);
        filtered_post_ = (1.0 - alpha) * filtered_post_ + alpha * post_activities;
        post_filtered = filtered_post_;
    }
    
    // Oja learning rule: Δw = η * a_post * (a_pre - a_post * w)
    for (int i = 0; i < weights.rows(); ++i) {
        for (int j = 0; j < weights.cols(); ++j) {
            if (j < post_filtered.size()) {
                double delta = learning_rate_ * post_filtered(j) * 
                              (pre_filtered(i) - post_filtered(j) * weights(i, j)) * dt;
                weights(i, j) += delta;
            }
        }
    }
}

// Voja Implementation

Voja::Voja(double learning_rate, std::shared_ptr<Synapse> post_synapse)
    : learning_rate_(learning_rate), post_synapse_(post_synapse), initialized_(false) {
    
    if (learning_rate < 0) {
        throw std::invalid_argument("Learning rate must be non-negative");
    }
}

void Voja::apply(Eigen::MatrixXd& weights,
                const Eigen::VectorXd& pre_activities,
                const Eigen::VectorXd& post_activities,
                double dt) {
    
    if (!initialized_) {
        filtered_post_ = Eigen::VectorXd::Zero(post_activities.size());
        initialized_ = true;
    }
    
    Eigen::VectorXd post_filtered = post_activities;
    
    // Filter post-synaptic activities if synapse is provided
    if (post_synapse_) {
        double alpha = dt / (post_synapse_->getTau() + dt);
        filtered_post_ = (1.0 - alpha) * filtered_post_ + alpha * post_activities;
        post_filtered = filtered_post_;
    }
    
    // Voja learning rule: Δe = η * a_post * (input - e · a_post)
    // This modifies encoders rather than decoders
    // For simplicity, we apply it to weights here
    for (int i = 0; i < weights.rows(); ++i) {
        for (int j = 0; j < weights.cols(); ++j) {
            if (j < post_filtered.size()) {
                // Simplified Voja rule applied to weights
                double input_projection = pre_activities(i);
                double current_projection = 0.0;
                for (int k = 0; k < weights.cols(); ++k) {
                    current_projection += weights(i, k) * post_filtered(k);
                }
                
                double delta = learning_rate_ * post_filtered(j) * 
                              (input_projection - current_projection) * dt;
                weights(i, j) += delta;
            }
        }
    }
}

} // namespace nengo