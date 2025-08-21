#pragma once

#include <string>
#include <memory>
#include <vector>
#include <optional>

namespace nengo {

/**
 * Base class for all Nengo objects.
 * Equivalent to nengo.base.NengoObject in Python version.
 */
class NengoObject {
public:
    explicit NengoObject(const std::string& label = "", 
                        std::optional<int> seed = std::nullopt);
    virtual ~NengoObject() = default;

    const std::string& getLabel() const { return label_; }
    void setLabel(const std::string& label) { label_ = label; }
    
    std::optional<int> getSeed() const { return seed_; }
    void setSeed(std::optional<int> seed) { seed_ = seed; }

    // String representation equivalent to Python __str__
    virtual std::string toString() const;
    virtual std::string repr() const;

    // Disable copy constructor and assignment operator
    NengoObject(const NengoObject&) = delete;
    NengoObject& operator=(const NengoObject&) = delete;

    // Enable move constructor and assignment operator
    NengoObject(NengoObject&&) = default;
    NengoObject& operator=(NengoObject&&) = default;

protected:
    std::string label_;
    std::optional<int> seed_;
};

/**
 * Base class for processes.
 * Equivalent to nengo.base.Process in Python version.
 */
class Process {
public:
    explicit Process(int default_size_in = 0, 
                    int default_size_out = 1, 
                    double default_dt = 0.001,
                    std::optional<int> seed = std::nullopt);
    virtual ~Process() = default;

    // Getters matching Python properties
    int getDefaultSizeIn() const { return default_size_in_; }
    int getDefaultSizeOut() const { return default_size_out_; }
    double getDefaultDt() const { return default_dt_; }
    std::optional<int> getSeed() const { return seed_; }

    // Setters 
    void setDefaultSizeIn(int size) { default_size_in_ = size; }
    void setDefaultSizeOut(int size) { default_size_out_ = size; }
    void setDefaultDt(double dt) { default_dt_ = dt; }
    void setSeed(std::optional<int> seed) { seed_ = seed; }

    // String representation
    virtual std::string toString() const;

protected:
    int default_size_in_;
    int default_size_out_;
    double default_dt_;
    std::optional<int> seed_;
};

} // namespace nengo