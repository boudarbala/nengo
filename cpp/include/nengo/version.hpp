#pragma once

#include <string>
#include <vector>

namespace nengo {

/**
 * Nengo version information.
 * 
 * We use semantic versioning (see http://semver.org/) and conform to PEP440.
 * '.dev0' will be added to the version unless the code base represents a
 * release version. Release versions are git tagged with the version.
 */
class Version {
public:
    // Version components
    static constexpr int MAJOR = 4;
    static constexpr int MINOR = 1;
    static constexpr int PATCH = 1;
    static constexpr int DEV = 0;
    
    static const std::string NAME;
    static const std::string COPYRIGHT;
    
    // Get version info as vector
    static std::vector<int> getVersionInfo();
    
    // Get formatted version string
    static std::string getVersion();
    
    // Get full version string with dev suffix if applicable
    static std::string getFullVersion();
    
    // Get copyright string
    static std::string getCopyright();
    
    // Get package name
    static std::string getName();
};

} // namespace nengo