#ifndef COVER_MEDIA_LOADER_DEMO_H
#define COVER_MEDIA_LOADER_DEMO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void cover_image_manager_init(void);
void cover_ui_scroll_range(uint32_t start_slot_id, uint32_t end_slot_id);
const char *cover_ui_get_image_path(uint32_t slot_id,
                                    uint32_t *out_content_id,
                                    const char **out_desc_str);
int cover_ui_has_more(void);
void cover_ui_reset_all(void);
void cover_ui_deinit_all(void);

#ifdef __cplusplus
}
#endif

#endif /* COVER_MEDIA_LOADER_DEMO_H */
