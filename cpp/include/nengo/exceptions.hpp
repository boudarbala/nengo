#pragma once

#include <stdexcept>
#include <string>
#include <optional>

namespace nengo {

/**
 * Base class for Nengo exceptions.
 * 
 * NengoException instances should not be created; this base class exists so
 * that all exceptions raised by Nengo can be caught in a try / catch block.
 */
class NengoException : public std::exception {
public:
    explicit NengoException(const std::string& message) : message_(message) {}
    const char* what() const noexcept override { return message_.c_str(); }

protected:
    std::string message_;
};

/**
 * A ValueError encountered during validation of a parameter.
 */
class ValidationError : public NengoException {
public:
    ValidationError(const std::string& msg, const std::string& attr, 
                   const std::string& obj = "");
    
    std::string toString() const;
    
    const std::string& getAttr() const { return attr_; }
    const std::string& getObj() const { return obj_; }

private:
    std::string attr_;
    std::string obj_;
};

/**
 * A RuntimeError raised when an algorithm does not converge.
 */
class ConvergenceError : public NengoException {
public:
    explicit ConvergenceError(const std::string& message) 
        : NengoException(message) {}
};

/**
 * A ValidationError occurring because a parameter is read-only.
 */
class ReadonlyError : public ValidationError {
public:
    ReadonlyError(const std::string& attr, const std::string& obj = "", 
                  const std::string& msg = "");
};

/**
 * A ValueError encountered during the build process.
 */
class BuildError : public NengoException {
public:
    explicit BuildError(const std::string& message) 
        : NengoException(message) {}
};

/**
 * A feature that has been removed in a backwards-incompatible way.
 */
class ObsoleteError : public NengoException {
public:
    ObsoleteError(const std::string& msg, const std::string& since = "", 
                  const std::string& url = "");
    
    std::string toString() const;
    
    const std::string& getSince() const { return since_; }
    const std::string& getUrl() const { return url_; }

private:
    std::string since_;
    std::string url_;
};

/**
 * A feature that has been moved elsewhere.
 */
class MovedError : public NengoException {
public:
    explicit MovedError(const std::string& location = "");
    
    std::string toString() const;

private:
    std::string location_;
};

/**
 * A ValueError encountered in the config system.
 */
class ConfigError : public NengoException {
public:
    explicit ConfigError(const std::string& message) 
        : NengoException(message) {}
};

/**
 * An error in how SPA keeps track of modules.
 */
class SpaModuleError : public NengoException {
public:
    explicit SpaModuleError(const std::string& message) 
        : NengoException(message) {}
};

/**
 * An error encountered while parsing a SPA expression.
 */
class SpaParseError : public NengoException {
public:
    explicit SpaParseError(const std::string& message) 
        : NengoException(message) {}
};

/**
 * Raised when attempting to run a closed simulator.
 */
class SimulatorClosed : public NengoException {
public:
    explicit SimulatorClosed(const std::string& message = "Simulator is closed") 
        : NengoException(message) {}
};

/**
 * An error encountered during simulation of the model.
 */
class SimulationError : public NengoException {
public:
    explicit SimulationError(const std::string& message) 
        : NengoException(message) {}
};

/**
 * An error dealing with Signals in the builder.
 */
class SignalError : public NengoException {
public:
    explicit SignalError(const std::string& message) 
        : NengoException(message) {}
};

/**
 * An error in fingerprinting an object for cache identification.
 */
class FingerprintError : public NengoException {
public:
    explicit FingerprintError(const std::string& message) 
        : NengoException(message) {}
};

/**
 * An error with the Network context stack.
 */
class NetworkContextError : public NengoException {
public:
    explicit NetworkContextError(const std::string& message) 
        : NengoException(message) {}
};

/**
 * Raised a requested network conversion cannot be done.
 */
class Unconvertible : public NengoException {
public:
    explicit Unconvertible(const std::string& message) 
        : NengoException(message) {}
};

/**
 * An IO error in reading from or writing to the decoder cache.
 */
class CacheIOError : public NengoException {
public:
    explicit CacheIOError(const std::string& message) 
        : NengoException(message) {}
};

/**
 * A timeout occurred while waiting for a resource.
 */
class TimeoutError : public NengoException {
public:
    explicit TimeoutError(const std::string& message) 
        : NengoException(message) {}
};

// Warning classes (C++ doesn't have warnings like Python, so these are just exceptions)

/**
 * Base class for Nengo warnings (implemented as exceptions in C++).
 */
class NengoWarning : public NengoException {
public:
    explicit NengoWarning(const std::string& message) 
        : NengoException(message) {}
};

/**
 * A NengoObject has not been added to a network.
 */
class NotAddedToNetworkWarning : public NengoWarning {
public:
    explicit NotAddedToNetworkWarning(const std::string& obj);
    
    std::string toString() const;

private:
    std::string obj_;
};

/**
 * A non-critical issue in accessing files in the cache.
 */
class CacheIOWarning : public NengoWarning {
public:
    explicit CacheIOWarning(const std::string& message) 
        : NengoWarning(message) {}
};

} // namespace nengo