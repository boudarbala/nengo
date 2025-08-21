#pragma once

#include "nengo/base.hpp"
#include <Eigen/Dense>
#include <vector>
#include <memory>

namespace nengo {

/**
 * Probes are used to collect data from objects during simulation.
 */
class Probe : public NengoObject {
public:
    /**
     * Create a probe to record data from an object.
     * @param target Object to probe (Ensemble, Node, or Connection)
     * @param attr Attribute to probe (not used in basic version)
     * @param sample_every Sampling period (not implemented in basic version)
     * @param label Label for the probe
     */
    explicit Probe(std::shared_ptr<NengoObject> target,
                   const std::string& attr = "",
                   double sample_every = 0.001,
                   const std::string& label = "");

    ~Probe() override = default;

    // Getters
    std::shared_ptr<NengoObject> getTarget() const { return target_; }
    double getSampleEvery() const { return sample_every_; }

    // Data collection
    void record(const Eigen::VectorXd& data);
    void record(double data);
    
    const std::vector<Eigen::VectorXd>& getData() const { return data_; }
    void clearData() { data_.clear(); }

    // Get data as matrix (time x dimensions)
    Eigen::MatrixXd getDataMatrix() const;

private:
    std::shared_ptr<NengoObject> target_;
    std::string attr_;
    double sample_every_;
    std::vector<Eigen::VectorXd> data_;
};

} // namespace nengo