#include "nengo/simulator.hpp"
#include "nengo/ensemble.hpp"
#include "nengo/node.hpp"
#include "nengo/connection.hpp"
#include "nengo/probe.hpp"
#include <stdexcept>

namespace nengo {

Simulator::Simulator(std::shared_ptr<Network> network, double dt)
    : network_(network), dt_(dt), time_(0.0), step_(0) {
    build();
}

void Simulator::step() {
    stepNodes();
    stepEnsembles();
    stepConnections();
    updateProbes();
    
    time_ += dt_;
    step_++;
}

void Simulator::run(double duration) {
    int steps = static_cast<int>(duration / dt_);
    runSteps(steps);
}

void Simulator::runSteps(int steps) {
    for (int i = 0; i < steps; ++i) {
        step();
    }
}

void Simulator::reset() {
    time_ = 0.0;
    step_ = 0;
    initializeStates();
    
    // Clear probe data
    for (const auto& probe : network_->getProbes()) {
        probe->clearData();
    }
}

const std::vector<Eigen::VectorXd>& Simulator::getData(std::shared_ptr<Probe> probe) const {
    return probe->getData();
}

Eigen::MatrixXd Simulator::getDataMatrix(std::shared_ptr<Probe> probe) const {
    return probe->getDataMatrix();
}

void Simulator::build() {
    initializeStates();
}

void Simulator::stepEnsembles() {
    for (const auto& ensemble : network_->getEnsembles()) {
        // Get input current to ensemble
        Eigen::VectorXd input = Eigen::VectorXd::Zero(ensemble->getDimensions());
        
        // Apply input from connections
        for (const auto& conn : network_->getConnections()) {
            if (conn->getPost() == ensemble) {
                Eigen::VectorXd pre_output = getObjectOutput(conn->getPre());
                Eigen::VectorXd conn_output = conn->compute(pre_output);
                input += conn_output;
            }
        }
        
        // Convert to neural input
        Eigen::VectorXd neural_input = ensemble->getEncoders() * input * ensemble->getRadius();
        neural_input = ensemble->getGain() * neural_input + ensemble->getBias();
        
        // Run neuron model
        Eigen::VectorXd neural_output(ensemble->getNeurons());
        ensemble->getNeuronType()->step(neural_input, neural_output);
        
        // Store state (simplified - just store the input for now)
        ensemble_states_[ensemble.get()] = input;
    }
}

void Simulator::stepNodes() {
    for (const auto& node : network_->getNodes()) {
        // Get input to node
        Eigen::VectorXd input = Eigen::VectorXd::Zero(node->getSizeIn());
        
        // Apply input from connections
        for (const auto& conn : network_->getConnections()) {
            if (conn->getPost() == node) {
                Eigen::VectorXd pre_output = getObjectOutput(conn->getPre());
                Eigen::VectorXd conn_output = conn->compute(pre_output);
                if (conn_output.size() == input.size()) {
                    input += conn_output;
                }
            }
        }
        
        // Run node
        Eigen::VectorXd output = node->run(time_, input);
        node_outputs_[node.get()] = output;
    }
}

void Simulator::stepConnections() {
    // Connections are processed in stepEnsembles and stepNodes
}

void Simulator::updateProbes() {
    for (const auto& probe : network_->getProbes()) {
        Eigen::VectorXd data = getObjectOutput(probe->getTarget());
        probe->record(data);
    }
}

void Simulator::initializeStates() {
    // Initialize ensemble states
    for (const auto& ensemble : network_->getEnsembles()) {
        ensemble_states_[ensemble.get()] = Eigen::VectorXd::Zero(ensemble->getDimensions());
    }
    
    // Initialize node outputs
    for (const auto& node : network_->getNodes()) {
        node_outputs_[node.get()] = Eigen::VectorXd::Zero(node->getSizeOut());
    }
}

Eigen::VectorXd Simulator::getObjectOutput(std::shared_ptr<NengoObject> obj) const {
    if (auto ensemble = std::dynamic_pointer_cast<Ensemble>(obj)) {
        auto it = ensemble_states_.find(ensemble.get());
        if (it != ensemble_states_.end()) {
            return it->second;
        }
        return Eigen::VectorXd::Zero(ensemble->getDimensions());
    } else if (auto node = std::dynamic_pointer_cast<Node>(obj)) {
        auto it = node_outputs_.find(node.get());
        if (it != node_outputs_.end()) {
            return it->second;
        }
        return Eigen::VectorXd::Zero(node->getSizeOut());
    }
    
    return Eigen::VectorXd();
}

} // namespace nengo