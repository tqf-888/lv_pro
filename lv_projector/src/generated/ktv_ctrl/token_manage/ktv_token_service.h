#ifndef KTV_TOKEN_SERVICE_H
#define KTV_TOKEN_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*
 * Token service:
 * - Fetch token/expire from server via KTV_REQ_FETCH_MEMORY
 * - Cache in static memory for fast reads
 * - Persist last good token+expire for offline fallback after reboot
 *
 * Failure behavior:
 * - If network fetch fails, returns cached value (persisted or in-memory).
 * - If cache is empty, returns an empty string (never NULL) to avoid crashes
 *   in URL builders that assume a valid pointer.
 */

void ktv_token_service_init(void);

/* Returns pointer to internal static buffer. Never NULL. */
const char *ktv_token_service_get(void);

/* Force refresh from network (retry inside). Returns 0 on success, -1 on failure. */
int ktv_token_service_refresh_now(void);

/* For diagnostics. */
uint32_t ktv_token_service_expire_time_sec(void);

#ifdef __cplusplus
}
#endif

#endif /* KTV_TOKEN_SERVICE_H */
