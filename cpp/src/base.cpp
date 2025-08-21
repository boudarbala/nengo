#include "nengo/base.hpp"

namespace nengo {

NengoObject::NengoObject(const std::string& label) : label_(label) {
    if (label_.empty()) {
        label_ = "NengoObject";
    }
}

Process::Process(double default_size_out) : default_size_out_(default_size_out) {}

} // namespace nengo