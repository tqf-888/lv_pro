#ifndef ARTIST_MEDIA_LOADER_DEMO_H
#define ARTIST_MEDIA_LOADER_DEMO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void demo_image_manager_init(void);
void demo_ui_scroll_range(uint32_t start_slot_id, uint32_t end_slot_id);
const char *demo_ui_get_image_path(uint32_t slot_id,
                                   uint32_t *out_content_id,
                                   const char **out_name_str);
int demo_ui_has_more(void);
void demo_ui_reset_all(void);
void demo_ui_deinit_all(void);

#ifdef __cplusplus
}
#endif

#endif /* ARTIST_MEDIA_LOADER_DEMO_H */
