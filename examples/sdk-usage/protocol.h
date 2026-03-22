/*
 * protocol.h — Shared protocol definition for the temperature sensor example.
 *
 * Both server and client include this header to agree on service IDs,
 * method IDs, notification IDs, and payload structures.
 *
 * In a real project you would version this header and keep it in a
 * shared location (or generate it from an IDL-like spec).
 */

#pragma once
#include <cstdint>

namespace temp {

// Service identifier (arbitrary, must match between server and client)
constexpr uint32_t kServiceId = 0x0001;

// RPC method IDs
constexpr uint32_t kMethodGetTemp      = 1;
constexpr uint32_t kMethodSetThreshold = 2;

// Notification IDs
constexpr uint32_t kNotifyOverTemp = 1;

// Payload for SetThreshold request
struct Thresholds {
    float high;
    float low;
};

static_assert(sizeof(Thresholds) == 8, "Thresholds must be 8 bytes");

} // namespace temp
