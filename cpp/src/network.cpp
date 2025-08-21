#include "nengo/network.hpp"
#include "nengo/ensemble.hpp"
#include "nengo/node.hpp"
#include "nengo/connection.hpp"
#include "nengo/probe.hpp"

namespace nengo {

Network::Network(const std::string& label)
    : NengoObject(label.empty() ? "Network" : label) {}

void Network::add(std::shared_ptr<Ensemble> ensemble) {
    ensembles_.push_back(ensemble);
}

void Network::add(std::shared_ptr<Node> node) {
    nodes_.push_back(node);
}

void Network::add(std::shared_ptr<Connection> connection) {
    connections_.push_back(connection);
}

void Network::add(std::shared_ptr<Probe> probe) {
    probes_.push_back(probe);
}

void Network::add(std::shared_ptr<Network> network) {
    networks_.push_back(network);
}

std::vector<std::shared_ptr<NengoObject>> Network::getAllObjects() const {
    std::vector<std::shared_ptr<NengoObject>> objects;
    
    for (const auto& ens : ensembles_) {
        objects.push_back(ens);
    }
    for (const auto& node : nodes_) {
        objects.push_back(node);
    }
    for (const auto& conn : connections_) {
        objects.push_back(conn);
    }
    for (const auto& probe : probes_) {
        objects.push_back(probe);
    }
    for (const auto& net : networks_) {
        objects.push_back(net);
        // Recursively add nested network objects
        auto nested = net->getAllObjects();
        objects.insert(objects.end(), nested.begin(), nested.end());
    }
    
    return objects;
}

} // namespace nengo