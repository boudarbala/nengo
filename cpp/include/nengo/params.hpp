#pragma once

#include <string>
#include <vector>
#include <functional>
#include <optional>
#include <type_traits>
#include <stdexcept>

namespace nengo {

/**
 * Base class for parameter validation and management.
 * 
 * This is a simplified version of the Python parameter system,
 * providing basic type checking and validation.
 */
template<typename T>
class Parameter {
public:
    /**
     * Constructor for Parameter.
     * 
     * Parameters
     * ----------
     * name : std::string
     *     Name of the parameter.
     * default_value : T, optional
     *     Default value for the parameter.
     * readonly : bool, optional
     *     Whether the parameter is read-only. Default is false.
     */
    explicit Parameter(const std::string& name, 
                      const T& default_value = T{}, 
                      bool readonly = false)
        : name_(name), default_value_(default_value), readonly_(readonly) {}
    
    virtual ~Parameter() = default;
    
    /**
     * Validate and coerce a value.
     * 
     * Parameters
     * ----------
     * value : T
     *     Value to validate.
     * 
     * Returns
     * -------
     * T
     *     Validated/coerced value.
     */
    virtual T validate(const T& value) const {
        return value;  // Base implementation does no validation
    }
    
    /**
     * Check if a value is valid.
     * 
     * Parameters
     * ----------
     * value : T
     *     Value to check.
     * 
     * Returns
     * -------
     * bool
     *     True if value is valid.
     */
    virtual bool isValid(const T& value) const {
        try {
            validate(value);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    // Getters
    const std::string& getName() const { return name_; }
    const T& getDefault() const { return default_value_; }
    bool isReadonly() const { return readonly_; }
    
    // Setters
    void setDefault(const T& value) { default_value_ = validate(value); }
    void setReadonly(bool readonly) { readonly_ = readonly; }

protected:
    std::string name_;
    T default_value_;
    bool readonly_;
};

/**
 * Parameter for numeric values with range checking.
 */
template<typename T>
class NumberParam : public Parameter<T> {
public:
    static_assert(std::is_arithmetic_v<T>, "NumberParam requires arithmetic type");
    
    /**
     * Constructor for NumberParam.
     * 
     * Parameters
     * ----------
     * name : std::string
     *     Name of the parameter.
     * default_value : T, optional
     *     Default value for the parameter.
     * low : std::optional<T>, optional
     *     Minimum allowed value.
     * high : std::optional<T>, optional
     *     Maximum allowed value.
     * readonly : bool, optional
     *     Whether the parameter is read-only. Default is false.
     */
    explicit NumberParam(const std::string& name,
                        const T& default_value = T{},
                        std::optional<T> low = std::nullopt,
                        std::optional<T> high = std::nullopt,
                        bool readonly = false)
        : Parameter<T>(name, default_value, readonly), low_(low), high_(high) {}
    
    T validate(const T& value) const override {
        if (low_.has_value() && value < low_.value()) {
            throw std::invalid_argument(this->name_ + " must be >= " + std::to_string(low_.value()));
        }
        if (high_.has_value() && value > high_.value()) {
            throw std::invalid_argument(this->name_ + " must be <= " + std::to_string(high_.value()));
        }
        return value;
    }
    
    // Getters
    std::optional<T> getLow() const { return low_; }
    std::optional<T> getHigh() const { return high_; }

private:
    std::optional<T> low_;
    std::optional<T> high_;
};

/**
 * Parameter for integer values.
 */
using IntParam = NumberParam<int>;

/**
 * Parameter for floating-point values.
 */
using FloatParam = NumberParam<double>;

/**
 * Parameter for boolean values.
 */
class BoolParam : public Parameter<bool> {
public:
    explicit BoolParam(const std::string& name,
                      bool default_value = false,
                      bool readonly = false)
        : Parameter<bool>(name, default_value, readonly) {}
    
    bool validate(const bool& value) const override {
        return value;  // All boolean values are valid
    }
};

/**
 * Parameter for string values with optional choices.
 */
class StringParam : public Parameter<std::string> {
public:
    /**
     * Constructor for StringParam.
     * 
     * Parameters
     * ----------
     * name : std::string
     *     Name of the parameter.
     * default_value : std::string, optional
     *     Default value for the parameter.
     * choices : std::vector<std::string>, optional
     *     Valid choices for the parameter.
     * readonly : bool, optional
     *     Whether the parameter is read-only. Default is false.
     */
    explicit StringParam(const std::string& name,
                        const std::string& default_value = "",
                        const std::vector<std::string>& choices = {},
                        bool readonly = false)
        : Parameter<std::string>(name, default_value, readonly), choices_(choices) {}
    
    std::string validate(const std::string& value) const override {
        if (!choices_.empty()) {
            bool found = false;
            for (const auto& choice : choices_) {
                if (value == choice) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                throw std::invalid_argument(name_ + " must be one of the valid choices");
            }
        }
        return value;
    }
    
    // Getters
    const std::vector<std::string>& getChoices() const { return choices_; }

private:
    std::vector<std::string> choices_;
};

/**
 * Parameter for enumerated values.
 */
template<typename E>
class EnumParam : public Parameter<E> {
public:
    static_assert(std::is_enum_v<E>, "EnumParam requires enum type");
    
    explicit EnumParam(const std::string& name,
                      const E& default_value = E{},
                      bool readonly = false)
        : Parameter<E>(name, default_value, readonly) {}
    
    E validate(const E& value) const override {
        // In a full implementation, you'd check against valid enum values
        return value;
    }
};

/**
 * A frozen object base class for immutable objects.
 */
class FrozenObject {
public:
    virtual ~FrozenObject() = default;
    
protected:
    // In a full implementation, this would prevent modification after construction
    bool frozen_ = false;
    
    void freeze() { frozen_ = true; }
    bool isFrozen() const { return frozen_; }
};

} // namespace nengo