#pragma once

#include <string>
#include <memory>
#include <vector>
#include <optional>

namespace nengo {

/**
 * Base class for all Nengo objects.
 * 
 * Equivalent to nengo.base.NengoObject in Python version.
 * 
 * Parameters
 * ----------
 * label : string, optional
 *     A descriptive label for the object (default: "").
 * seed : int, optional
 *     Random number seed (default: None).
 * 
 * Attributes
 * ----------
 * label : string
 *     A descriptive label for the object.
 * seed : int or None
 *     Random number seed.
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
 * A general system with input, output, and state.
 * 
 * Equivalent to nengo.base.Process in Python version.
 * 
 * Parameters
 * ----------
 * default_size_in : int, optional
 *     Sets the default size in for nodes using this process (default: 0).
 * default_size_out : int, optional
 *     Sets the default size out for nodes running this process (default: 1).
 * default_dt : double, optional
 *     Default simulation timestep (default: 0.001).
 * seed : int, optional
 *     Random number seed. Ensures random factors will be the same each run
 *     (default: None).
 * 
 * Attributes
 * ----------
 * default_size_in : int
 *     The default size in for nodes using this process.
 * default_size_out : int
 *     The default size out for nodes running this process.
 * default_dt : double
 *     Default simulation timestep.
 * seed : int or None
 *     Random number seed.
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