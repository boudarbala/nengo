#pragma once

#include "nengo/config.hpp"
#include "nengo/ensemble.hpp"
#include <memory>

namespace nengo {

/**
 * Configuration presets for common use cases.
 */
namespace presets {

/**
 * Configuration preset for a thresholding ensemble.
 * 
 * This preset adjust ensemble parameters for thresholding. The ensemble's
 * neurons will only fire for values above threshold. One can either decode
 * the represented value (if it is above the threshold) or decode
 * a step function if binary classification is desired.
 * 
 * This preset:
 * 
 * - Sets intercepts to be between `threshold` and `radius` with an
 *   exponential distribution (shape parameter of `intercept_width`).
 *   This clusters intercepts near the threshold for better approximation.
 * - Sets encoders to 1.
 * - Sets evaluation points to be uniformly distributed between
 *   `threshold` and `radius`.
 * - Sets the radius.
 * 
 * Parameters
 * ----------
 * threshold : double
 *     Point at which ensembles should start firing.
 * intercept_width : double, optional
 *     Controls how widely distributed the intercepts are. Smaller values
 *     give more clustering at the threshold, larger values give a more
 *     uniform distribution.
 * radius : double, optional
 *     Ensemble radius.
 * 
 * Returns
 * -------
 * std::shared_ptr<Config>
 *     Configuration with presets.
 */
std::shared_ptr<Config> ThresholdingEnsembles(double threshold, 
                                             double intercept_width = 0.15, 
                                             double radius = 1.0);

} // namespace presets

} // namespace nengo