#pragma once

#include <string>
#include <map>
#include <memory>
#include <functional>
#include <fstream>
#include <sstream>

namespace nengo {

/**
 * A basic cache system for storing computed data.
 * 
 * This is a simplified version of the Python cache system,
 * providing basic file-based caching capabilities.
 */
class Cache {
public:
    /**
     * Constructor for Cache.
     * 
     * Parameters
     * ----------
     * cache_dir : std::string, optional
     *     Directory to store cache files. Default is "./cache".
     * enabled : bool, optional
     *     Whether caching is enabled. Default is true.
     */
    explicit Cache(const std::string& cache_dir = "./cache", bool enabled = true);
    
    /**
     * Store an item in the cache.
     * 
     * Parameters
     * ----------
     * key : std::string
     *     Unique key for the cached item.
     * data : std::string
     *     Data to cache (serialized).
     * 
     * Returns
     * -------
     * bool
     *     True if successfully cached.
     */
    bool put(const std::string& key, const std::string& data);
    
    /**
     * Retrieve an item from the cache.
     * 
     * Parameters
     * ----------
     * key : std::string
     *     Unique key for the cached item.
     * 
     * Returns
     * -------
     * std::string
     *     Cached data, or empty string if not found.
     */
    std::string get(const std::string& key) const;
    
    /**
     * Check if an item exists in the cache.
     * 
     * Parameters
     * ----------
     * key : std::string
     *     Unique key for the cached item.
     * 
     * Returns
     * -------
     * bool
     *     True if item exists in cache.
     */
    bool has(const std::string& key) const;
    
    /**
     * Remove an item from the cache.
     * 
     * Parameters
     * ----------
     * key : std::string
     *     Unique key for the cached item.
     * 
     * Returns
     * -------
     * bool
     *     True if successfully removed.
     */
    bool remove(const std::string& key);
    
    /**
     * Clear all items from the cache.
     */
    void clear();
    
    /**
     * Get cache statistics.
     * 
     * Returns
     * -------
     * std::pair<size_t, size_t>
     *     (hits, misses) counts.
     */
    std::pair<size_t, size_t> getStats() const;
    
    /**
     * Enable or disable the cache.
     */
    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }
    
    /**
     * Get the cache directory.
     */
    const std::string& getCacheDir() const { return cache_dir_; }

private:
    std::string cache_dir_;
    bool enabled_;
    mutable size_t hits_;
    mutable size_t misses_;
    
    std::string getFilePath(const std::string& key) const;
    bool ensureCacheDir() const;
    std::string hashKey(const std::string& key) const;
};

/**
 * Decoder cache for storing computed decoders.
 * 
 * Specialized cache for neural decoders to avoid recomputation.
 */
class DecoderCache : public Cache {
public:
    /**
     * Constructor for DecoderCache.
     * 
     * Parameters
     * ----------
     * cache_dir : std::string, optional
     *     Directory to store cache files. Default is "./decoder_cache".
     * enabled : bool, optional
     *     Whether caching is enabled. Default is true.
     * readonly : bool, optional
     *     Whether cache is read-only. Default is false.
     */
    explicit DecoderCache(const std::string& cache_dir = "./decoder_cache",
                         bool enabled = true, bool readonly = false);
    
    /**
     * Store decoders in the cache.
     * 
     * Parameters
     * ----------
     * fingerprint : std::string
     *     Unique fingerprint for the decoder computation.
     * decoders : std::string
     *     Serialized decoder data.
     * 
     * Returns
     * -------
     * bool
     *     True if successfully cached.
     */
    bool putDecoders(const std::string& fingerprint, const std::string& decoders);
    
    /**
     * Retrieve decoders from the cache.
     * 
     * Parameters
     * ----------
     * fingerprint : std::string
     *     Unique fingerprint for the decoder computation.
     * 
     * Returns
     * -------
     * std::string
     *     Cached decoders, or empty string if not found.
     */
    std::string getDecoders(const std::string& fingerprint) const;
    
    /**
     * Check if decoders exist in the cache.
     * 
     * Parameters
     * ----------
     * fingerprint : std::string
     *     Unique fingerprint for the decoder computation.
     * 
     * Returns
     * -------
     * bool
     *     True if decoders exist in cache.
     */
    bool hasDecoders(const std::string& fingerprint) const;
    
    /**
     * Set read-only mode.
     */
    void setReadonly(bool readonly) { readonly_ = readonly; }
    bool isReadonly() const { return readonly_; }

private:
    bool readonly_;
};

// Global decoder cache instance
extern DecoderCache decoder_cache;

} // namespace nengo