#include "ktv_persist_util.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int ktv_persist_read_file(const char *path, void *buf, size_t buf_size)
{
    FILE *fp;
    size_t n;

    if (path == NULL || path[0] == '\0' || buf == NULL || buf_size == 0U) {
        return -1;
    }

    fp = fopen(path, "rb");
    if (fp == NULL) {
        return -1;
    }

    n = fread(buf, 1U, buf_size, fp);
    fclose(fp);

    if (n == 0U) {
        return -1;
    }

    return (int)n;
}

int ktv_persist_write_file_atomic(const char *path, const void *data, size_t len)
{
    char tmp_path[512];
    FILE *fp;
    size_t n;
    int rc;

    if (path == NULL || path[0] == '\0' || data == NULL || len == 0U) {
        return -1;
    }

    rc = snprintf(tmp_path, sizeof(tmp_path), "%s.tmp", path);
    if (rc <= 0 || (size_t)rc >= sizeof(tmp_path)) {
        return -1;
    }

    fp = fopen(tmp_path, "wb");
    if (fp == NULL) {
        return -1;
    }

    n = fwrite(data, 1U, len, fp);
    if (n != len) {
        fclose(fp);
        unlink(tmp_path);
        return -1;
    }

    /* Best-effort flush: still ok if fsync is not available on target FS. */
    fflush(fp);
    if (fileno(fp) >= 0) {
        (void)fsync(fileno(fp));
    }
    fclose(fp);

    if (rename(tmp_path, path) != 0) {
        unlink(tmp_path);
        return -1;
    }

    return 0;
}

