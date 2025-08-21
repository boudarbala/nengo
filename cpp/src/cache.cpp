#include "nengo/cache.hpp"
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace nengo {

// Cache Implementation

Cache::Cache(const std::string& cache_dir, bool enabled)
    : cache_dir_(cache_dir), enabled_(enabled), hits_(0), misses_(0) {
    
    if (enabled_) {
        ensureCacheDir();
    }
}

bool Cache::put(const std::string& key, const std::string& data) {
    if (!enabled_) {
        return false;
    }
    
    if (!ensureCacheDir()) {
        return false;
    }
    
    std::string filepath = getFilePath(key);
    std::ofstream file(filepath, std::ios::binary);
    
    if (!file.is_open()) {
        return false;
    }
    
    file << data;
    return file.good();
}

std::string Cache::get(const std::string& key) const {
    if (!enabled_) {
        misses_++;
        return "";
    }
    
    std::string filepath = getFilePath(key);
    std::ifstream file(filepath, std::ios::binary);
    
    if (!file.is_open()) {
        misses_++;
        return "";
    }
    
    std::ostringstream buffer;
    buffer << file.rdbuf();
    
    if (file.good()) {
        hits_++;
        return buffer.str();
    } else {
        misses_++;
        return "";
    }
}

bool Cache::has(const std::string& key) const {
    if (!enabled_) {
        return false;
    }
    
    std::string filepath = getFilePath(key);
    return std::filesystem::exists(filepath);
}

bool Cache::remove(const std::string& key) {
    if (!enabled_) {
        return false;
    }
    
    std::string filepath = getFilePath(key);
    if (std::filesystem::exists(filepath)) {
        return std::filesystem::remove(filepath);
    }
    return true;  // Already doesn't exist
}

void Cache::clear() {
    if (!enabled_) {
        return;
    }
    
    try {
        if (std::filesystem::exists(cache_dir_)) {
            for (const auto& entry : std::filesystem::directory_iterator(cache_dir_)) {
                if (entry.is_regular_file()) {
                    std::filesystem::remove(entry.path());
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error clearing cache: " << e.what() << std::endl;
    }
}

std::pair<size_t, size_t> Cache::getStats() const {
    return {hits_, misses_};
}

std::string Cache::getFilePath(const std::string& key) const {
    std::string hashed_key = hashKey(key);
    return cache_dir_ + "/" + hashed_key + ".cache";
}

bool Cache::ensureCacheDir() const {
    try {
        if (!std::filesystem::exists(cache_dir_)) {
            return std::filesystem::create_directories(cache_dir_);
        }
        return true;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error creating cache directory: " << e.what() << std::endl;
        return false;
    }
}

std::string Cache::hashKey(const std::string& key) const {
    // Simple hash function - in practice, you'd want a proper hash
    std::hash<std::string> hasher;
    size_t hash_value = hasher(key);
    
    std::ostringstream oss;
    oss << std::hex << hash_value;
    return oss.str();
}

// DecoderCache Implementation

DecoderCache::DecoderCache(const std::string& cache_dir, bool enabled, bool readonly)
    : Cache(cache_dir, enabled), readonly_(readonly) {}

bool DecoderCache::putDecoders(const std::string& fingerprint, const std::string& decoders) {
    if (readonly_) {
        return false;
    }
    return put("decoders_" + fingerprint, decoders);
}

std::string DecoderCache::getDecoders(const std::string& fingerprint) const {
    return get("decoders_" + fingerprint);
}

bool DecoderCache::hasDecoders(const std::string& fingerprint) const {
    return has("decoders_" + fingerprint);
}

// Global decoder cache instance
DecoderCache decoder_cache;

} // namespace nengo