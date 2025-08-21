#pragma once

#include "nengo/network.hpp"
#include "nengo/ensemble.hpp"
#include "nengo/node.hpp"
#include "nengo/connection.hpp"
#include "nengo/synapses.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <optional>
#include <string>

namespace nengo {
namespace networks {

/**
 * An array of ensembles.
 * 
 * Equivalent to nengo.networks.EnsembleArray in Python version.
 * 
 * This acts, in some ways, like a single high-dimensional ensemble,
 * but actually consists of many sub-ensembles, each one representing
 * a separate dimension. This tends to be much faster to create
 * and can be more accurate than having one huge high-dimensional ensemble.
 * However, since the neurons represent different dimensions separately,
 * we cannot compute nonlinear interactions between those dimensions.
 * 
 * Parameters
 * ----------
 * n_neurons : int
 *     The number of neurons in each sub-ensemble.
 * n_ensembles : int
 *     The number of sub-ensembles to create.
 * ens_dimensions : int, optional
 *     The dimensionality of each sub-ensemble (default: 1).
 * label : std::string, optional
 *     A name to assign this EnsembleArray.
 *     Used for visualization and debugging.
 * seed : int, optional
 *     Random number seed that will be used in the build step.
 *     
 * Attributes
 * ----------
 * dimensions_per_ensemble : int
 *     The dimensionality of each sub-ensemble.
 * ea_ensembles : vector<shared_ptr<Ensemble>>
 *     The sub-ensembles in the ensemble array.
 * input : shared_ptr<Node>
 *     A node that provides input to all of the ensembles in the array.
 * n_ensembles : int
 *     The number of sub-ensembles to create.
 * n_neurons_per_ensemble : int
 *     The number of neurons in each sub-ensemble.
 * neuron_input : shared_ptr<Node>
 *     A node that provides input to all the neurons in the ensemble array.
 *     nullptr unless created in addNeuronInput().
 * neuron_output : shared_ptr<Node>
 *     A node that gathers neural output from all the neurons in the ensemble
 *     array. nullptr unless created in addNeuronOutput().
 * output : shared_ptr<Node>
 *     A node that gathers decoded output from all of the ensembles
 *     in the array.
 */
class EnsembleArray : public Network {
public:
    /**
     * Constructor for EnsembleArray.
     * 
     * @param n_neurons Number of neurons in each sub-ensemble
     * @param n_ensembles Number of sub-ensembles to create  
     * @param ens_dimensions Dimensionality of each sub-ensemble (default: 1)
     * @param label Name for this EnsembleArray (default: "")
     * @param seed Random number seed (default: nullopt)
     */
    explicit EnsembleArray(int n_neurons,
                          int n_ensembles,
                          int ens_dimensions = 1,
                          const std::string& label = "",
                          std::optional<int> seed = std::nullopt);
                          
    virtual ~EnsembleArray() = default;

    // Getters
    int getDimensions() const { return n_ensembles_ * dimensions_per_ensemble_; }
    int getDimensionsPerEnsemble() const { return dimensions_per_ensemble_; }
    int getNEnsembles() const { return n_ensembles_; }
    int getNeuronsPerEnsemble() const { return n_neurons_per_ensemble_; }
    
    const std::vector<std::shared_ptr<Ensemble>>& getEaEnsembles() const { return ea_ensembles_; }
    std::shared_ptr<Node> getInput() const { return input_; }
    std::shared_ptr<Node> getOutput() const { return output_; }
    std::shared_ptr<Node> getNeuronInput() const { return neuron_input_; }
    std::shared_ptr<Node> getNeuronOutput() const { return neuron_output_; }

    /**
     * Adds a node that provides input to the neurons of all ensembles.
     * 
     * Direct neuron input is useful for inhibiting the activity of all
     * neurons in the ensemble array.
     * 
     * This node is accessible through the neuron_input attribute
     * of this ensemble array.
     * 
     * @return Shared pointer to the neuron input node
     */
    std::shared_ptr<Node> addNeuronInput();

    /**
     * Adds a node that collects the neural output of all ensembles.
     * 
     * Direct neuron output is useful for plotting the spike raster of
     * all neurons in the ensemble array.
     * 
     * This node is accessible through the neuron_output attribute
     * of this ensemble array.
     * 
     * @return Shared pointer to the neuron output node
     */
    std::shared_ptr<Node> addNeuronOutput();

    /**
     * Adds a node that collects the decoded output of all ensembles.
     * 
     * By default, this is called once in constructor with function=nullptr.
     * However, this can be called multiple times with different functions,
     * similar to the way in which an ensemble can be connected to many
     * downstream ensembles with different functions.
     * 
     * @param name The name of the output. This will also be the name of the attribute
     *             set on the ensemble array.
     * @param function The function to compute across the connection from sub-ensembles
     *                to the new output node. If nullptr, identity function is used.
     * @param synapse The synapse model with which to filter the connections from
     *               sub-ensembles to the new output node.
     * @return Shared pointer to the output node
     */
    std::shared_ptr<Node> addOutput(const std::string& name,
                                   std::function<Eigen::VectorXd(const Eigen::VectorXd&)> function = nullptr,
                                   std::shared_ptr<Synapse> synapse = nullptr);

    // String representation
    std::string toString() const override;

private:
    int n_neurons_per_ensemble_;
    int n_ensembles_;
    int dimensions_per_ensemble_;
    
    std::vector<std::shared_ptr<Ensemble>> ea_ensembles_;
    std::shared_ptr<Node> input_;
    std::shared_ptr<Node> output_;
    std::shared_ptr<Node> neuron_input_;
    std::shared_ptr<Node> neuron_output_;
};

/**
 * Network for computing the element-wise product of inputs.
 * 
 * Equivalent to nengo.networks.Product in Python version.
 * 
 * This network uses the fact that we can implement element-wise multiplication
 * using the formula (a+b)^2 - (a-b)^2 = 4ab, which uses three squaring
 * operations that can be computed by an ensemble.
 * 
 * Parameters
 * ----------
 * n_neurons : int
 *     Number of neurons per ensemble in the product network.
 * dimensions : int
 *     Number of dimensions in each input.
 * input_magnitude : double, optional
 *     The expected magnitude of the input values. This can be used to set
 *     the radius of the ensembles to account for the range of the values
 *     (default: 1.0).
 * label : std::string, optional
 *     A name to assign this Product network.
 * seed : int, optional
 *     Random number seed.
 */
class Product : public Network {
public:
    explicit Product(int n_neurons,
                    int dimensions,
                    double input_magnitude = 1.0,
                    const std::string& label = "",
                    std::optional<int> seed = std::nullopt);
                    
    virtual ~Product() = default;

    // Getters
    int getDimensions() const { return dimensions_; }
    std::shared_ptr<Node> getInputA() const { return input_a_; }
    std::shared_ptr<Node> getInputB() const { return input_b_; }
    std::shared_ptr<Node> getOutput() const { return output_; }

    std::string toString() const override;

private:
    int dimensions_;
    std::shared_ptr<Node> input_a_;
    std::shared_ptr<Node> input_b_;
    std::shared_ptr<Node> output_;
    std::shared_ptr<EnsembleArray> sq1_;
    std::shared_ptr<EnsembleArray> sq2_;
};

} // namespace networks
} // namespace nengo