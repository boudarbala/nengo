#pragma once

#include "nengo/base.hpp"
#include <vector>
#include <memory>

namespace nengo {

// Forward declarations
class Ensemble;
class Node;
class Connection;
class Probe;

/**
 * A network contains ensembles, nodes, connections, and other networks.
 * 
 * A network is primarily used for grouping together related
 * objects and connections for visualization purposes.
 * However, you can also use networks as a nice way to reuse
 * network creation code.
 */
class Network : public NengoObject {
public:
    explicit Network(const std::string& label = "");
    ~Network() override = default;

    // Add objects to the network
    void add(std::shared_ptr<Ensemble> ensemble);
    void add(std::shared_ptr<Node> node);
    void add(std::shared_ptr<Connection> connection);
    void add(std::shared_ptr<Probe> probe);
    void add(std::shared_ptr<Network> network);

    // Getters for network components
    const std::vector<std::shared_ptr<Ensemble>>& getEnsembles() const { return ensembles_; }
    const std::vector<std::shared_ptr<Node>>& getNodes() const { return nodes_; }
    const std::vector<std::shared_ptr<Connection>>& getConnections() const { return connections_; }
    const std::vector<std::shared_ptr<Probe>>& getProbes() const { return probes_; }
    const std::vector<std::shared_ptr<Network>>& getNetworks() const { return networks_; }

    // Get all objects
    std::vector<std::shared_ptr<NengoObject>> getAllObjects() const;

private:
    std::vector<std::shared_ptr<Ensemble>> ensembles_;
    std::vector<std::shared_ptr<Node>> nodes_;
    std::vector<std::shared_ptr<Connection>> connections_;
    std::vector<std::shared_ptr<Probe>> probes_;
    std::vector<std::shared_ptr<Network>> networks_;
};

} // namespace nengo