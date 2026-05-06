#ifndef LV_RICH_SONG_DEBUG_H
#define LV_RICH_SONG_DEBUG_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*
 * 日志总开关说明：
 * 1. LV_RICH_SONG_ALLOC_LOG_ENABLE 控制内存申请/释放日志
 * 2. LV_RICH_SONG_FLOW_LOG_ENABLE  控制普通流程日志
 * 3. LV_RICH_SONG_HOT_LOG_ENABLE   控制高频路径日志（默认关闭，避免刷屏）
 *
 * 建议：
 * - 排查泄漏/高频申请时：把 ALLOC 打开
 * - 日常联调：保留 FLOW，关闭 HOT
 * - 正式版本：三个都可改成 0
 */
#ifndef LV_RICH_SONG_ALLOC_LOG_ENABLE
#define LV_RICH_SONG_ALLOC_LOG_ENABLE 1
#endif

#ifndef LV_RICH_SONG_FLOW_LOG_ENABLE
#define LV_RICH_SONG_FLOW_LOG_ENABLE 1
#endif

#ifndef LV_RICH_SONG_HOT_LOG_ENABLE
#define LV_RICH_SONG_HOT_LOG_ENABLE 0
#endif

#define RS_LOG_PREFIX "[rich_song] "

#if LV_RICH_SONG_FLOW_LOG_ENABLE
#define RS_FLOW_LOG(fmt, ...) \
    do { printf(RS_LOG_PREFIX fmt "\n", ##__VA_ARGS__); } while (0)
#else
#define RS_FLOW_LOG(fmt, ...) do { } while (0)
#endif

#if LV_RICH_SONG_HOT_LOG_ENABLE
#define RS_HOT_LOG(fmt, ...) \
    do { printf(RS_LOG_PREFIX fmt "\n", ##__VA_ARGS__); } while (0)
#else
#define RS_HOT_LOG(fmt, ...) do { } while (0)
#endif

static inline void *rs_dbg_malloc_impl(size_t size,
                                       const char *tag,
                                       const char *file,
                                       int line)
{
    void *p = malloc(size);
#if LV_RICH_SONG_ALLOC_LOG_ENABLE
    printf(RS_LOG_PREFIX "ALLOC malloc tag=%s size=%lu ptr=%p @%s:%d\n",
           (tag != NULL) ? tag : "",
           (unsigned long)size,
           p,
           file,
           line);
#endif
    return p;
}

static inline void *rs_dbg_calloc_impl(size_t n,
                                       size_t size,
                                       const char *tag,
                                       const char *file,
                                       int line)
{
    void *p = calloc(n, size);
#if LV_RICH_SONG_ALLOC_LOG_ENABLE
    printf(RS_LOG_PREFIX "ALLOC calloc tag=%s n=%lu size=%lu total=%lu ptr=%p @%s:%d\n",
           (tag != NULL) ? tag : "",
           (unsigned long)n,
           (unsigned long)size,
           (unsigned long)(n * size),
           p,
           file,
           line);
#endif
    return p;
}

static inline void rs_dbg_free_impl(void *p,
                                    const char *tag,
                                    const char *file,
                                    int line)
{
#if LV_RICH_SONG_ALLOC_LOG_ENABLE
    printf(RS_LOG_PREFIX "FREE tag=%s ptr=%p @%s:%d\n",
           (tag != NULL) ? tag : "",
           p,
           file,
           line);
#endif
    free(p);
}

#define RS_MALLOC(size, tag) rs_dbg_malloc_impl((size), (tag), __FILE__, __LINE__)
#define RS_CALLOC(n, size, tag) rs_dbg_calloc_impl((n), (size), (tag), __FILE__, __LINE__)
#define RS_FREE(ptr, tag) \
    do { \
        void *rs_free_tmp__ = (void *)(ptr); \
        rs_dbg_free_impl(rs_free_tmp__, (tag), __FILE__, __LINE__); \
        (ptr) = NULL; \
    } while (0)

#endif
