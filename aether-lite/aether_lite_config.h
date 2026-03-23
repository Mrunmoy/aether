#ifndef AETHER_LITE_CONFIG_H
#define AETHER_LITE_CONFIG_H

/* Max payload size (determines RX/TX buffer sizes).
 * Must be >= largest struct in the IDL. */
#define AL_MAX_PAYLOAD          256

/* Max registered services (typically 1). */
#define AL_MAX_SERVICES         1

/* Max methods per service. */
#define AL_MAX_METHODS          8

/* CRC implementation: 1 = 1KB lookup table (fast), 0 = bit-by-bit (small) */
#define AL_CRC_USE_TABLE        1

/* Partial frame receive timeout in milliseconds.
 * If a frame is not fully received within this time, discard and re-sync.
 * 0 = disabled. */
#define AL_RX_TIMEOUT_MS        500

/* Enable notification sending. 0 saves ~200B flash. */
#define AL_ENABLE_NOTIFICATIONS 1

/* Enable diagnostic error counters. 0 saves ~12B RAM. */
#define AL_ENABLE_DIAGNOSTICS   1

#endif /* AETHER_LITE_CONFIG_H */
