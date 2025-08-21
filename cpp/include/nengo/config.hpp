#pragma once

#include <map>
#include <string>
#include <any>
#include <typeinfo>
#include <memory>
#include <vector>

namespace nengo {

/**
 * A simplified configuration system for C++ Nengo.
 * 
 * This is a basic implementation to store default values and configuration
 * options for Nengo objects. It's simplified compared to the Python version.
 */
class Config {
public:
    Config() = default;
    virtual ~Config() = default;
    
    // Store a configuration value by name
    template<typename T>
    void set(const std::string& key, const T& value) {
        config_values_[key] = value;
    }
    
    // Get a configuration value by name
    template<typename T>
    T get(const std::string& key, const T& default_value = T{}) const {
        auto it = config_values_.find(key);
        if (it != config_values_.end()) {
            try {
                return std::any_cast<T>(it->second);
            } catch (const std::bad_any_cast&) {
                return default_value;
            }
        }
        return default_value;
    }
    
    // Check if a key exists
    bool has(const std::string& key) const {
        return config_values_.find(key) != config_values_.end();
    }
    
    // Remove a configuration value
    void remove(const std::string& key) {
        config_values_.erase(key);
    }
    
    // Clear all configuration values
    void clear() {
        config_values_.clear();
    }
    
    // Get all keys
    std::vector<std::string> keys() const {
        std::vector<std::string> result;
        for (const auto& pair : config_values_) {
            result.push_back(pair.first);
        }
        return result;
    }

private:
    std::map<std::string, std::any> config_values_;
};

/**
 * Class-specific configuration holder.
 * 
 * This stores configuration values specific to a particular class type.
 */
template<typename T>
class ClassConfig : public Config {
public:
    explicit ClassConfig() = default;
    
    // Set ensemble-specific defaults
    void setRadius(double radius) { set("radius", radius); }
    void setIntercepts(const std::string& dist) { set("intercepts", dist); }
    void setEncoders(const std::string& encoders) { set("encoders", encoders); }
    void setEvalPoints(const std::string& eval_points) { set("eval_points", eval_points); }
    
    // Get ensemble-specific values
    double getRadius(double default_val = 1.0) const { 
        return get<double>("radius", default_val); 
    }
    std::string getIntercepts(const std::string& default_val = "") const { 
        return get<std::string>("intercepts", default_val); 
    }
    std::string getEncoders(const std::string& default_val = "") const { 
        return get<std::string>("encoders", default_val); 
    }
    std::string getEvalPoints(const std::string& default_val = "") const { 
        return get<std::string>("eval_points", default_val); 
    }
};

// Forward declaration for Ensemble
class Ensemble;

// Convenient type alias
using EnsembleConfig = ClassConfig<Ensemble>;

} // namespace nengo