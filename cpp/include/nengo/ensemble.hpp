#pragma once

#include "nengo/base.hpp"
#include "nengo/neurons.hpp"
#include <Eigen/Dense>
#include <memory>

namespace nengo {

/**
 * A group of neurons that collectively represent a vector.
 * 
 * Parameters:
 * - n_neurons: The number of neurons
 * - dimensions: The number of representational dimensions
 * - radius: The representational radius of the ensemble
 * - neuron_type: The model that simulates all neurons in the ensemble
 */
class Ensemble : public NengoObject {
public:
    Ensemble(int n_neurons, int dimensions, double radius = 1.0,
             std::shared_ptr<NeuronType> neuron_type = nullptr,
             const std::string& label = "");

    ~Ensemble() override = default;

    // Getters
    int getNeurons() const { return n_neurons_; }
    int getDimensions() const { return dimensions_; }
    double getRadius() const { return radius_; }
    std::shared_ptr<NeuronType> getNeuronType() const { return neuron_type_; }

    // Encoder/decoder matrices
    const Eigen::MatrixXd& getEncoders() const { return encoders_; }
    void setEncoders(const Eigen::MatrixXd& encoders);

    const Eigen::MatrixXd& getGain() const { return gain_; }
    const Eigen::VectorXd& getBias() const { return bias_; }
    
    void setGainBias(const Eigen::MatrixXd& gain, const Eigen::VectorXd& bias);

    // Neuron representation
    class Neurons {
    public:
        explicit Neurons(Ensemble* ensemble) : ensemble_(ensemble) {}
        Ensemble* getEnsemble() const { return ensemble_; }
        int size() const { return ensemble_->getNeurons(); }
    private:
        Ensemble* ensemble_;
    };

    Neurons neurons;

private:
    int n_neurons_;
    int dimensions_;
    double radius_;
    std::shared_ptr<NeuronType> neuron_type_;
    
    // Neural parameters
    Eigen::MatrixXd encoders_;  // (n_neurons, dimensions)
    Eigen::MatrixXd gain_;      // (n_neurons, 1)
    Eigen::VectorXd bias_;      // (n_neurons,)
    
    void initializeDefaults();
};

} // namespace nengo