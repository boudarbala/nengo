#include "nengo/base.hpp"
#include <sstream>

namespace nengo {

NengoObject::NengoObject(const std::string& label, std::optional<int> seed) 
    : label_(label), seed_(seed) {
    if (label_.empty()) {
        label_ = "NengoObject";
    }
}

std::string NengoObject::toString() const {
    std::string label_part;
    if (label_.empty() || label_ == "NengoObject") {
        label_part = " (unlabeled)";
    } else {
        label_part = " '" + label_ + "'";
    }
    return "<" + std::string(typeid(*this).name()) + label_part + ">";
}

std::string NengoObject::repr() const {
    std::ostringstream oss;
    oss << "<" << typeid(*this).name() << " at 0x" << std::hex << this << ">";
    return oss.str();
}

Process::Process(int default_size_in, int default_size_out, double default_dt, std::optional<int> seed)
    : default_size_in_(default_size_in), 
      default_size_out_(default_size_out),
      default_dt_(default_dt),
      seed_(seed) {}

std::string Process::toString() const {
    std::ostringstream oss;
    oss << "<" << typeid(*this).name() 
        << " size_in=" << default_size_in_
        << " size_out=" << default_size_out_
        << " dt=" << default_dt_ << ">";
    return oss.str();
}

} // namespace nengo