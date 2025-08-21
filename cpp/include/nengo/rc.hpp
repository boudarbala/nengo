#pragma once

#include <string>
#include <map>
#include <cstdint>

namespace nengo {

/**
 * Nengo runtime configuration settings.
 * 
 * This class provides access to globally configured features of Nengo.
 * In C++, this is simplified compared to the Python version.
 */
class RC {
public:
    // Precision settings
    struct Precision {
        int bits = 64;
    };
    
    // Decoder cache settings
    struct DecoderCache {
        bool enabled = true;
        bool readonly = false;
        std::string size = "512 MB";
        std::string path = "./cache";
    };
    
    // Progress bar settings
    struct Progress {
        std::string progress_bar = "auto";
    };
    
    // Exception settings
    struct Exceptions {
        bool simplified = true;
    };
    
    // Simulator settings
    struct Simulator {
        bool fail_fast = false;
    };
    
    // Get singleton instance
    static RC& getInstance();
    
    // Access to configuration sections
    Precision& precision() { return precision_; }
    const Precision& precision() const { return precision_; }
    
    DecoderCache& decoder_cache() { return decoder_cache_; }
    const DecoderCache& decoder_cache() const { return decoder_cache_; }
    
    Progress& progress() { return progress_; }
    const Progress& progress() const { return progress_; }
    
    Exceptions& exceptions() { return exceptions_; }
    const Exceptions& exceptions() const { return exceptions_; }
    
    Simulator& simulator() { return simulator_; }
    const Simulator& simulator() const { return simulator_; }
    
    // Convenience methods
    std::string getFloatDtype() const;
    std::string getIntDtype() const;
    
    // Reset to defaults
    void reset();
    
    // Load configuration from file (simplified)
    bool loadFromFile(const std::string& filename);

private:
    RC() = default;
    
    Precision precision_;
    DecoderCache decoder_cache_;
    Progress progress_;
    Exceptions exceptions_;
    Simulator simulator_;
};

// Global RC instance
extern RC& rc;

} // namespace nengo