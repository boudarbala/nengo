#pragma once

// Include all Nengo C++ headers
#include "nengo/base.hpp"
#include "nengo/network.hpp"
#include "nengo/ensemble.hpp"
#include "nengo/node.hpp"
#include "nengo/connection.hpp"
#include "nengo/probe.hpp"
#include "nengo/simulator.hpp"
#include "nengo/neurons.hpp"

/**
 * Nengo C++ - A neural simulation library
 * 
 * This is a C++ translation of the Python Nengo library,
 * implementing the Neural Engineering Framework (NEF).
 * 
 * Basic usage:
 * 
 * ```cpp
 * #include <nengo/nengo.hpp>
 * 
 * auto network = std::make_shared<nengo::Network>();
 * auto ensemble = std::make_shared<nengo::Ensemble>(100, 2);
 * network->add(ensemble);
 * 
 * nengo::Simulator sim(network);
 * sim.run(1.0);  // Run for 1 second
 * ```
 */

namespace nengo {
    // Version information
    constexpr const char* VERSION = "1.0.0";
    constexpr const char* DESCRIPTION = "Neural simulation library implementing the NEF";
}