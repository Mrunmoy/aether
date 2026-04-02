#ifndef SENSOR_DEVICE_H
#define SENSOR_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Run the simulated sensor device on the given file descriptor.
 * Blocks until *stop_flag becomes non-zero.
 * Intended to be called from a dedicated thread. */
void sensor_device_run(int fd, const volatile int *stop_flag);

#ifdef __cplusplus
}
#endif

#endif /* SENSOR_DEVICE_H */
