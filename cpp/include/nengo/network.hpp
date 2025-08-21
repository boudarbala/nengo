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
 * Equivalent to nengo.network.Network in Python version.
 * 
 * A network is primarily used for grouping together related
 * objects and connections for visualization purposes.
 * However, you can also use networks as a nice way to reuse
 * network creation code.
 * 
 * Parameters
 * ----------
 * label : string, optional
 *     A descriptive label for the network (default: "").
 * seed : int, optional
 *     Random number seed that will be fed to the random number generator.
 *     Setting the seed makes the network's build process deterministic
 *     (default: None).
 * 
 * Attributes
 * ----------
 * ensembles : vector<Ensemble>
 *     Ensemble instances in this network.
 * nodes : vector<Node>
 *     Node instances in this network.
 * connections : vector<Connection>
 *     Connection instances in this network.
 * probes : vector<Probe>
 *     Probe instances in this network.
 * networks : vector<Network>
 *     Network instances in this network.
 */
class Network : public NengoObject {
public:
    explicit Network(const std::string& label = "",
                    std::optional<int> seed = std::nullopt);
    ~Network() override = default;

    // Add objects to the network
    void add(std::shared_ptr<Ensemble> ensemble);
    void add(std::shared_ptr<Node> node);
    void add(std::shared_ptr<Connection> connection);
    void add(std::shared_ptr<Probe> probe);
    void add(std::shared_ptr<Network> network);

    // Getters for network components (matching Python properties)
    const std::vector<std::shared_ptr<Ensemble>>& getEnsembles() const { return ensembles_; }
    const std::vector<std::shared_ptr<Node>>& getNodes() const { return nodes_; }
    const std::vector<std::shared_ptr<Connection>>& getConnections() const { return connections_; }
    const std::vector<std::shared_ptr<Probe>>& getProbes() const { return probes_; }
    const std::vector<std::shared_ptr<Network>>& getNetworks() const { return networks_; }

    // Python-style property accessors
    const auto& ensembles() const { return ensembles_; }
    const auto& nodes() const { return nodes_; }
    const auto& connections() const { return connections_; }
    const auto& probes() const { return probes_; }
    const auto& networks() const { return networks_; }

    // Get all objects
    std::vector<std::shared_ptr<NengoObject>> getAllObjects() const;

    // String representation
    std::string toString() const override;

private:
    std::vector<std::shared_ptr<Ensemble>> ensembles_;
    std::vector<std::shared_ptr<Node>> nodes_;
    std::vector<std::shared_ptr<Connection>> connections_;
    std::vector<std::shared_ptr<Probe>> probes_;
    std::vector<std::shared_ptr<Network>> networks_;
};

} // namespace nengo