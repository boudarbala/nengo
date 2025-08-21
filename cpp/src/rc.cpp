#include "nengo/rc.hpp"
#include <fstream>
#include <iostream>

namespace nengo {

RC& RC::getInstance() {
    static RC instance;
    return instance;
}

std::string RC::getFloatDtype() const {
    return "float" + std::to_string(precision_.bits);
}

std::string RC::getIntDtype() const {
    return "int" + std::to_string(precision_.bits);
}

void RC::reset() {
    // Reset to default values
    precision_.bits = 64;
    
    decoder_cache_.enabled = true;
    decoder_cache_.readonly = false;
    decoder_cache_.size = "512 MB";
    decoder_cache_.path = "./cache";
    
    progress_.progress_bar = "auto";
    
    exceptions_.simplified = true;
    
    simulator_.fail_fast = false;
}

bool RC::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Simplified configuration file parsing
    // In a full implementation, this would parse INI-style config files
    std::string line;
    std::string current_section;
    
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }
        
        // Check for section headers [section]
        if (line[0] == '[' && line.back() == ']') {
            current_section = line.substr(1, line.length() - 2);
            continue;
        }
        
        // Parse key=value pairs
        size_t eq_pos = line.find('=');
        if (eq_pos != std::string::npos) {
            std::string key = line.substr(0, eq_pos);
            std::string value = line.substr(eq_pos + 1);
            
            // Trim whitespace (simplified)
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            // Apply settings based on section
            if (current_section == "precision") {
                if (key == "bits") {
                    precision_.bits = std::stoi(value);
                }
            } else if (current_section == "decoder_cache") {
                if (key == "enabled") {
                    decoder_cache_.enabled = (value == "true" || value == "True" || value == "1");
                } else if (key == "readonly") {
                    decoder_cache_.readonly = (value == "true" || value == "True" || value == "1");
                } else if (key == "size") {
                    decoder_cache_.size = value;
                } else if (key == "path") {
                    decoder_cache_.path = value;
                }
            } else if (current_section == "progress") {
                if (key == "progress_bar") {
                    progress_.progress_bar = value;
                }
            } else if (current_section == "exceptions") {
                if (key == "simplified") {
                    exceptions_.simplified = (value == "true" || value == "True" || value == "1");
                }
            } else if (current_section == "nengo.Simulator") {
                if (key == "fail_fast") {
                    simulator_.fail_fast = (value == "true" || value == "True" || value == "1");
                }
            }
        }
    }
    
    return true;
}

// Global RC instance
RC& rc = RC::getInstance();

} // namespace nengo