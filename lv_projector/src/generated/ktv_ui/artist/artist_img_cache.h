#ifndef ARTIST_IMG_CACHE_H
#define ARTIST_IMG_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "ktv_image_manager.h"

int artist_img_cache_init(void);
void artist_img_cache_deinit(void);
void artist_img_cache_clear(void);
void artist_img_cache_set_page_size(uint32_t page_size);

/* 纯 id 驱动：artist 层只传 artist_id。 */
int artist_img_cache_prefetch(uint32_t artist_id);
ktv_img_state_t artist_img_cache_state(uint32_t artist_id);
const char *artist_img_cache_pull_tls(uint32_t artist_id);

#ifdef __cplusplus
}
#endif

#endif
