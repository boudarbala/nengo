#include "nengo/exceptions.hpp"
#include <sstream>

namespace nengo {

ValidationError::ValidationError(const std::string& msg, const std::string& attr, 
                               const std::string& obj)
    : NengoException(msg), attr_(attr), obj_(obj) {}

std::string ValidationError::toString() const {
    if (obj_.empty()) {
        return attr_ + ": " + message_;
    }
    return obj_ + "." + attr_ + ": " + message_;
}

ReadonlyError::ReadonlyError(const std::string& attr, const std::string& obj, 
                           const std::string& msg) 
    : ValidationError(msg.empty() ? attr + " is read-only and cannot be changed" : msg, 
                     attr, obj) {}

ObsoleteError::ObsoleteError(const std::string& msg, const std::string& since, 
                           const std::string& url)
    : NengoException(msg), since_(since), url_(url) {}

std::string ObsoleteError::toString() const {
    std::string since_txt = since_.empty() ? "" : " since " + since_;
    std::string url_txt = url_.empty() ? "" : "\nFor more information, please visit " + url_;
    return "Obsolete" + since_txt + ": " + message_ + url_txt;
}

MovedError::MovedError(const std::string& location) 
    : NengoException("This feature has been moved"), location_(location) {}

std::string MovedError::toString() const {
    return "This feature has been moved to " + location_;
}

NotAddedToNetworkWarning::NotAddedToNetworkWarning(const std::string& obj)
    : NengoWarning("Object not added to network"), obj_(obj) {}

std::string NotAddedToNetworkWarning::toString() const {
    std::ostringstream oss;
    oss << obj_ << " was not added to the network. When copying objects, "
        << "use the copy method on the object instead of Python's copy "
        << "module. When unpickling objects, they have to be added to "
        << "networks manually.";
    return oss.str();
}

} // namespace nengo