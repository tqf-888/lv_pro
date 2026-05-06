#ifndef KTV_TIME_SERVICE_H
#define KTV_TIME_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*
 * Time service:
 * - Sync current epoch time from server via KTV_REQ_FETCH_MEMORY
 * - Maintain an estimated real time using (server_time + CLOCK_MONOTONIC delta)
 * - Persist last good server epoch (seconds) for offline fallback after reboot
 *
 * This module has NO UI dependency.
 */

/* Load persisted snapshot and initialize internal time base (best-effort). */
void ktv_time_service_init(void);

/* Fetch from server immediately. Returns 0 on success, -1 on failure. */
int ktv_time_service_sync_now(void);

/* Start/stop background sync+log thread (best-effort). */
int ktv_time_service_thread_start(void);
void ktv_time_service_thread_stop(void);

/* Get estimated current epoch time (seconds). Never blocks. */
uint32_t ktv_time_service_now_sec(void);

/*
 * Optional hook for presentation/business layers.
 * Called from time thread context when a minute log tick happens (and also
 * after successful/failed sync events when the thread decides to print).
 */
typedef void (*ktv_time_service_on_tick_cb)(uint32_t now_sec, const char *reason, void *user);
void ktv_time_service_set_tick_hook(ktv_time_service_on_tick_cb cb, void *user);

#ifdef __cplusplus
}
#endif

#endif /* KTV_TIME_SERVICE_H */
