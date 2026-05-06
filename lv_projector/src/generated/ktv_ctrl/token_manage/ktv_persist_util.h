/*
 * Small persistence helpers for KTV modules.
 *
 * Goal:
 * - Keep business modules free from file I/O details.
 * - Provide best-effort, atomic-ish save for small blobs.
 *
 * Notes:
 * - This code runs on embedded Linux in production. Paths such as
 *   "/usr/share/lv_projector/..." are expected to be writable by the app.
 * - Persistence is best-effort: failures must not break runtime behavior.
 */
#ifndef KTV_PERSIST_UTIL_H
#define KTV_PERSIST_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/* Read full file into buf. Returns number of bytes read, or -1 on failure. */
int ktv_persist_read_file(const char *path, void *buf, size_t buf_size);

/* Write data to path using temp + rename. Returns 0 on success, -1 on failure. */
int ktv_persist_write_file_atomic(const char *path, const void *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* KTV_PERSIST_UTIL_H */
