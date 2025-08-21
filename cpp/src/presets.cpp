#include "nengo/presets.hpp"
#include "nengo/config.hpp"

namespace nengo {
namespace presets {

std::shared_ptr<Config> ThresholdingEnsembles(double threshold, 
                                             double intercept_width, 
                                             double radius) {
    auto config = std::make_shared<EnsembleConfig>();
    
    // Set radius
    config->setRadius(radius);
    
    // Set intercepts to be exponentially distributed between threshold and radius
    // This is a simplified representation - in a full implementation,
    // this would create actual distribution objects
    config->setIntercepts("Exponential(" + std::to_string(intercept_width) + 
                         ", " + std::to_string(threshold) + 
                         ", " + std::to_string(radius) + ")");
    
    // Set encoders to 1
    config->setEncoders("Choice([[1]])");
    
    // Set evaluation points uniformly distributed between threshold/radius and 1
    config->setEvalPoints("Uniform(" + std::to_string(threshold / radius) + ", 1)");
    
    return config;
}

} // namespace presets
} // namespace nengo