#pragma once

#include "nengo/network.hpp"
#include "nengo/ensemble.hpp"
#include "nengo/node.hpp"
#include "nengo/connection.hpp"
#include "nengo/probe.hpp"
#include <memory>
#include <unordered_map>

namespace nengo {

/**
 * Simulator runs a Nengo model.
 * This is a simplified version that implements basic functionality.
 */
class Simulator {
public:
    /**
     * Create a simulator for a network.
     * @param network Network to simulate
     * @param dt Simulation timestep
     */
    explicit Simulator(std::shared_ptr<Network> network, double dt = 0.001);
    
    ~Simulator() = default;

    // Simulation control
    void step();
    void run(double duration);
    void runSteps(int steps);
    void reset();

    // Getters
    double getTime() const { return time_; }
    double getDt() const { return dt_; }
    int getStep() const { return step_; }
    std::shared_ptr<Network> getNetwork() const { return network_; }

    // Get probe data
    const std::vector<Eigen::VectorXd>& getData(std::shared_ptr<Probe> probe) const;
    Eigen::MatrixXd getDataMatrix(std::shared_ptr<Probe> probe) const;

private:
    std::shared_ptr<Network> network_;
    double dt_;
    double time_;
    int step_;
    
    // Internal state
    std::unordered_map<Ensemble*, Eigen::VectorXd> ensemble_states_;
    std::unordered_map<Node*, Eigen::VectorXd> node_outputs_;
    
    // Build the simulation
    void build();
    void stepEnsembles();
    void stepNodes();
    void stepConnections();
    void updateProbes();
    
    // Helper functions
    void initializeStates();
    Eigen::VectorXd getObjectOutput(std::shared_ptr<NengoObject> obj) const;
};

} // namespace nengo