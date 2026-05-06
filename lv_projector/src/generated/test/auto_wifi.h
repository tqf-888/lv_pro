/**
 * @file auto_wifi.h
 * @brief Automatically start Wi-Fi on boot if enabled in settings.
 *
 * This module loads Wi-Fi kernel modules and initializes STA mode
 * in a background thread, based on persistent configuration.
 */

#ifndef AUTO_WIFI_H
#define AUTO_WIFI_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    WIFI_SRART,
    WIFI_SCAN,
    WIFI_CONNECT,
} WIFIProcType;

/**
 * @brief Start automatic Wi-Fi initialization in a detached background thread.
 *
 * This function:
 *   - Loads Wi-Fi kernel modules via script,
 *   - Checks if Wi-Fi is enabled in config file,
 *   - Initializes and enables Wi-Fi STA mode if allowed.
 *
 * The thread is detached — no need to join or manage it.
 *
 * @return 0 on successful thread creation, -1 on failure.
 */
int auto_wifi_start(void);
void *_wifi_proc(WIFIProcType type);

#ifdef __cplusplus
}
#endif

#endif // AUTO_WIFI_H