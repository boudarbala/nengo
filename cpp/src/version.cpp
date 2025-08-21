#include "nengo/version.hpp"
#include <sstream>

namespace nengo {

const std::string Version::NAME = "nengo";
const std::string Version::COPYRIGHT = "Copyright (c) 2013-2025 Nengo Developers";

std::vector<int> Version::getVersionInfo() {
    return {MAJOR, MINOR, PATCH};
}

std::string Version::getVersion() {
    std::ostringstream oss;
    auto version_info = getVersionInfo();
    
    for (size_t i = 0; i < version_info.size(); ++i) {
        if (i > 0) oss << ".";
        oss << version_info[i];
    }
    
    return oss.str();
}

std::string Version::getFullVersion() {
    std::string version = getVersion();
    
    if (DEV != 0) {
        version += ".dev" + std::to_string(DEV);
    }
    
    return version;
}

std::string Version::getCopyright() {
    return COPYRIGHT;
}

std::string Version::getName() {
    return NAME;
}

} // namespace nengo