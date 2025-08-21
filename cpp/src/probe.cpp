#include "nengo/probe.hpp"

namespace nengo {

Probe::Probe(std::shared_ptr<NengoObject> target,
             const std::string& attr,
             double sample_every,
             const std::string& label)
    : NengoObject(label.empty() ? "Probe" : label),
      target_(target),
      attr_(attr),
      sample_every_(sample_every) {}

void Probe::record(const Eigen::VectorXd& data) {
    data_.push_back(data);
}

void Probe::record(double data) {
    Eigen::VectorXd vec(1);
    vec << data;
    record(vec);
}

Eigen::MatrixXd Probe::getDataMatrix() const {
    if (data_.empty()) {
        return Eigen::MatrixXd();
    }
    
    int time_steps = data_.size();
    int dimensions = data_[0].size();
    
    Eigen::MatrixXd matrix(time_steps, dimensions);
    for (int t = 0; t < time_steps; ++t) {
        matrix.row(t) = data_[t].transpose();
    }
    
    return matrix;
}

} // namespace nengo