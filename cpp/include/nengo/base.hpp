#pragma once

#include <string>
#include <memory>
#include <vector>

namespace nengo {

/**
 * Base class for all Nengo objects.
 * Equivalent to nengo.base.NengoObject in Python version.
 */
class NengoObject {
public:
    explicit NengoObject(const std::string& label = "");
    virtual ~NengoObject() = default;

    const std::string& getLabel() const { return label_; }
    void setLabel(const std::string& label) { label_ = label; }

    // Disable copy constructor and assignment operator
    NengoObject(const NengoObject&) = delete;
    NengoObject& operator=(const NengoObject&) = delete;

    // Enable move constructor and assignment operator
    NengoObject(NengoObject&&) = default;
    NengoObject& operator=(NengoObject&&) = default;

protected:
    std::string label_;
};

/**
 * Base class for processes.
 * Equivalent to nengo.base.Process in Python version.
 */
class Process {
public:
    explicit Process(double default_size_out = 1.0);
    virtual ~Process() = default;

    virtual double getDefaultSizeOut() const { return default_size_out_; }

protected:
    double default_size_out_;
};

} // namespace nengo