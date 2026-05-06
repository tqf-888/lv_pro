#ifndef __KTV_SUBTITLE_FETCH_API_H__
#define __KTV_SUBTITLE_FETCH_API_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>

/*
 * 将 zrc 字幕地址落地到本地文件。
 * 1. subtitle_url 为空时，成功返回 1，但 out_local_path 为空字符串
 * 2. subtitle_url 为本地路径时，直接原样返回
 * 3. subtitle_url 为 http/https 时，下载到本地缓存路径后返回
 */
int ktv_fetch_subtitle_to_local(int song_id,
                                const char *subtitle_url,
                                char *out_local_path,
                                size_t out_size);

#ifdef __cplusplus
}
#endif

#endif
