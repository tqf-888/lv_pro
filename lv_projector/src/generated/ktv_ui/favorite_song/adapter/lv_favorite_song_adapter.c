#include "lv_favorite_song_adapter.h"

/*
 * 这个文件的核心思路：
 *
 * 1. get_item 时按需触发批次请求
 * 2. 异步线程下载 json 后，只上报“哪个批次好了”
 * 3. UI 线程定时器再把 json 读入 catalog，并刷新 vlist
 * 4. 用 request_epoch 隔离 reset 前后的新旧请求
 *
 * 为什么一定要加 request_epoch：
 * - reset 前 batch 0 请求已经发出
 * - reset 后又重新发了 batch 0
 * - 如果旧请求晚回来，只靠 batch_index=0 根本分不清新旧
 * - 所以请求发出时要带 epoch，回调时也要带 epoch
 * - UI 线程发现回调 epoch != 当前 epoch，直接丢弃
 */
#include "lv_renderer_favorite_song.h"
#include "../lv_favorite_song_debug.h"
#include "cJSON.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "gui_guider.h"
#include "db_list_pro_worker.h"
#include "ktv_player_ui.h"
#include "ktv_cloud_order_api.h"
#include "db_list_pro_thread.h"
#include "recommend_video_fetch.h"

LV_FONT_DECLARE(lv_font_Regular_20);
LV_FONT_DECLARE(lv_font_ktv_30);

static lv_favorite_song_adapter_t *g_favorite_song_default = NULL;

typedef struct {
    uint32_t data_count;
    uint32_t json_total_count;
    uint8_t json_total_count_valid;
    uint8_t reached_end;
    uint32_t final_total_count;
} rs_batch_sync_meta_t;

/*
 * 仅用于“必须 > 0”的配置项。
 *
 * 重要警示：
 * - overscan_rows_front / overscan_rows_back / preload_before / preload_after
 *   这些字段的 0 可能是业务上“合法且有意义”的值，不能在这里统一兜底。
 * - 之前把 overscan_rows_front=0 也强行兜成 1，结果首屏第 0 行会被当成
 *   “前向预渲染行”顶到可视区上方，直接导致初始化时首行丢失。
 * - 所以后面只允许 visible_rows / visible_cols / batch_size 这类“绝不能为 0”
 *   的字段走这个函数。
 */
static uint32_t rs_safe_positive_u32(uint32_t v, uint32_t defv)
{
    return (v == 0U) ? defv : v;
}

/*
 * 生成批次文件路径。
 * 新逻辑优先使用带 epoch 的文件名：
 *   /tmp/favorite_song_batch_<epoch>_<batch>.json
 * 这样旧请求和新请求不会写到同一个文件里。
 *
 * 下面保留旧路径兜底：
 *   /tmp/favorite_song_batch_<batch>.json
 */
static int rs_batch_build_path(uint32_t batch_index, uint32_t request_epoch, char *buf, size_t size)
{
    int n;

    if (buf == NULL || size == 0U) return -1;

    if (request_epoch != 0U) {
        n = snprintf(buf, size, "/tmp/favorite_song_batch_%u_%u.json", request_epoch, batch_index);
        if (n >= 0 && (size_t)n < size) {
            return 0;
        }
    }

    n = snprintf(buf, size, "/tmp/favorite_song_batch_%u.json", batch_index);
    if (n < 0 || (size_t)n >= size) return -1;
    return 0;
}

/* 把整个 json 文件一次性读到内存 */
static int rs_read_text_file(const char *path, char **out_buf, long *out_len)
{
    FILE *fp;
    long len;
    char *buf;

    if (out_buf == NULL || out_len == NULL || path == NULL) return -1;
    *out_buf = NULL;
    *out_len = 0;

    fp = fopen(path, "rb");
    if (fp == NULL) {
        RS_FLOW_LOG("read file open failed: path=%s", path);
        return -1;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return -1;
    }
    len = ftell(fp);
    if (len <= 0) {
        RS_FLOW_LOG("read file empty: path=%s len=%ld", path, len);
        fclose(fp);
        return -1;
    }
    if (fseek(fp, 0, SEEK_SET) != 0) {
        fclose(fp);
        return -1;
    }

    buf = (char *)malloc((size_t)len + 1U);
    if (buf == NULL) {
        fclose(fp);
        return -1;
    }

    if (fread(buf, 1, (size_t)len, fp) != (size_t)len) {
        free(buf);
        fclose(fp);
        return -1;
    }

    fclose(fp);
    buf[len] = '\0';

    *out_buf = buf;
    *out_len = len;
    RS_FLOW_LOG("read file ok: path=%s len=%ld", path, len);
    return 0;
}

/* 给歌曲行填默认文字样式 */
static void rs_fill_default_text_style(lv_favorite_song_item_t *it)
{
    if (it == NULL) return;

    it->color_idx = 0xFFFFFF;
    it->opa_idx = 255;
    it->font_idx = &lv_font_Regular_20;

    it->color_name = 0xFFFFFF;
    it->opa_name = 255;
    it->font_name = &lv_font_Regular_20;

    it->color_artist = 0xBBBBBB;
    it->opa_artist = 255;
    it->font_artist = &lv_font_Regular_20;

    it->color_a = 0xFFFF00;
    it->opa_a = 255;
    it->font_a = &lv_font_ktv_30;

    it->color_b = 0xFFFF00;
    it->opa_b = 255;
    it->font_b = &lv_font_ktv_30;

    it->color_c = 0xAAAAAA;
    it->opa_c = 255;
    it->font_c = &lv_font_ktv_30;

    it->color_d = 0x66CCFF;
    it->opa_d = 220;
    it->font_d = &lv_font_ktv_30;

    it->color_e = 0xFF99CC;
    it->opa_e = 200;
    it->font_e = &lv_font_ktv_30;

    it->color_f = 0x99FF99;
    it->opa_f = 180;
    it->font_f = &lv_font_ktv_30;
}

/*
 * 把某个批次对应的 json 文件同步到 catalog。
 * 注意这里会按 request_epoch 找文件。
 * 所以即使旧请求晚回来，它写的是旧 epoch 文件，
 * 当前这一轮只会读取当前 epoch 对应的文件。
 */
static int rs_sync_batch_from_json(lv_favorite_song_adapter_t *adapter,
                                   uint32_t batch_index,
                                   uint32_t request_epoch,
                                   rs_batch_sync_meta_t *meta)
{
    char path[128];
    char *json_str = NULL;
    long file_len = 0;
    cJSON *root = NULL;
    cJSON *data_obj = NULL;
    cJSON *list = NULL;
    cJSON *json_total_count = NULL;
    uint32_t start_id, end_id;
    int array_size, i;

    if (meta != NULL) {
        memset(meta, 0, sizeof(*meta));
    }

    if (adapter == NULL) {
        RS_FLOW_LOG("sync json failed: adapter null");
        return -1;
    }
    if (rs_batch_build_path(batch_index, request_epoch, path, sizeof(path)) != 0) {
        RS_FLOW_LOG("sync json path build failed: batch=%u epoch=%u", batch_index, request_epoch);
        return -1;
    }
    RS_FLOW_LOG("sync json start: batch=%u epoch=%u path=%s", batch_index, request_epoch, path);
    if (rs_read_text_file(path, &json_str, &file_len) != 0) {
        RS_FLOW_LOG("sync json read file failed: batch=%u epoch=%u path=%s", batch_index, request_epoch, path);
        return -1;
    }

    root = cJSON_Parse(json_str);
    free(json_str);
    if (root == NULL) {
        RS_FLOW_LOG("sync json parse failed: batch=%u epoch=%u path=%s", batch_index, request_epoch, path);
        return -1;
    }

    data_obj = cJSON_GetObjectItem(root, "data");
    if (!cJSON_IsObject(data_obj)) {
        RS_FLOW_LOG("sync json missing data object: batch=%u epoch=%u", batch_index, request_epoch);
        cJSON_Delete(root);
        return -1;
    }

    list = cJSON_GetObjectItem(data_obj, "list");
    if (!cJSON_IsArray(list)) {
        RS_FLOW_LOG("sync json missing data.list array: batch=%u epoch=%u", batch_index, request_epoch);
        cJSON_Delete(root);
        return -1;
    }

    json_total_count = cJSON_GetObjectItem(data_obj, "total");
    if (meta != NULL && json_total_count != NULL && cJSON_IsNumber(json_total_count) && json_total_count->valueint >= 0) {
        meta->json_total_count = (uint32_t)json_total_count->valueint;
        meta->json_total_count_valid = 1U;
    }

    start_id = batch_index * adapter->batch_size;
    end_id = start_id + adapter->batch_size;
    if (end_id > adapter->total_count) end_id = adapter->total_count;

    array_size = cJSON_GetArraySize(list);
    RS_FLOW_LOG("sync json array ok: batch=%u epoch=%u array_size=%d total_count=%u valid=%u",
                batch_index, request_epoch, array_size,
                (meta != NULL) ? meta->json_total_count : 0U,
                (meta != NULL) ? meta->json_total_count_valid : 0U);
    if (meta != NULL) {
        meta->data_count = (array_size > 0) ? (uint32_t)array_size : 0U;
    }

    for (i = 0; i < array_size && (start_id + (uint32_t)i) < end_id; ++i) {
        cJSON *node = cJSON_GetArrayItem(list, i);
        cJSON *songid = cJSON_GetObjectItem(node, "songid");
        cJSON *sname = cJSON_GetObjectItem(node, "songName");
        cJSON *artist = cJSON_GetObjectItem(node, "artist");
        cJSON *is_vip = cJSON_GetObjectItem(node, "isVip");
        cJSON *mv_id = cJSON_GetObjectItem(node, "mvId");
        cJSON *aiMv = cJSON_GetObjectItem(node, "aiMv");
        
        lv_favorite_song_item_t item;
        int vip_flag = 0;
        int mv_flag = 0;

        memset(&item, 0, sizeof(item));
        rs_fill_default_text_style(&item);
        item.slot_id = start_id + (uint32_t)i;
        item.song_id = (songid != NULL && cJSON_IsNumber(songid))
                     ? (uint32_t)songid->valueint
                     : item.slot_id;

        item.ready = 1U;
        item.loading = 0U;
        item.selected = 0U;

        snprintf(item.index_text, sizeof(item.index_text), "%u", item.slot_id + 1U);

        if (sname != NULL && cJSON_IsString(sname) && sname->valuestring != NULL) {
            snprintf(item.name, sizeof(item.name), "%s", sname->valuestring);
        } else {
            snprintf(item.name, sizeof(item.name), "未知");
        }

        if (artist != NULL && cJSON_IsString(artist) && artist->valuestring != NULL) {
            snprintf(item.artist, sizeof(item.artist), "%s", artist->valuestring);
        } else {
            item.artist[0] = '\0';
        }

        if (is_vip != NULL && cJSON_IsNumber(is_vip)) {
            vip_flag = is_vip->valueint;
        }

        if (mv_id != NULL && cJSON_IsNumber(mv_id)) {
            mv_flag = mv_id->valueint;
        }

        item.text_a[0] = '\0';
        item.text_b[0] = '\0';

        if (vip_flag) {
            snprintf(item.text_a, sizeof(item.text_a), "0");
        } else{
            snprintf(item.text_a, sizeof(item.text_a), "6");
            item.color_a = 0x7e7e7e;
        }

        if (mv_flag) {
            snprintf(item.text_b, sizeof(item.text_b), "7");
            item.color_b = 0xa600ff;
        }
        /* aiMv 为 null、缺字段或非 string 时不填 URL；勿对 NULL 节点读 valuestring */
        if (aiMv != NULL && cJSON_IsString(aiMv) && aiMv->valuestring != NULL) {
            snprintf(item.text_b, sizeof(item.text_b), "7");
            snprintf(item.ai_mv_url, sizeof(item.ai_mv_url), "%s", aiMv->valuestring);
            item.color_b = 0xff6500;
        }

        snprintf(item.text_c, sizeof(item.text_c), "2");
        snprintf(item.text_d, sizeof(item.text_d), "3");
        snprintf(item.text_e, sizeof(item.text_e), "4");


        
        RS_FLOW_LOG("sync item: batch=%u slot=%u song_id=%u name=%s artist=%s vip=%d mv=%d",
                    batch_index, item.slot_id, item.song_id, item.name, item.artist, vip_flag, mv_flag);
        (void)lv_favorite_song_catalog_set_item_by_slot(&adapter->catalog, item.slot_id, &item);
    }

    if (meta != NULL) {
        uint32_t received = (array_size > 0) ? (uint32_t)array_size : 0U;

        /*
         * 末页判定规则：
         * 1. 返回空 data，说明这一批开始已经没有数据；
         * 2. 返回条数 < batch_size，说明这一批就是最后一页；
         *
         * 一旦命中，最终总数就是：当前批次起始下标 + 实际返回条数。
         * 例如 batch_size=50：
         * - batch 3 返回 12 条 => 最终总数 = 3*50 + 12 = 162
         * - batch 0 返回 0 条  => 最终总数 = 0
         */
        if (received < adapter->batch_size) {
            meta->reached_end = 1U;
            meta->final_total_count = start_id + received;
        }
    }

    RS_FLOW_LOG("sync json done: batch=%u epoch=%u loaded=%u reached_end=%u final_total=%u",
                batch_index, request_epoch,
                (meta != NULL) ? meta->data_count : 0U,
                (meta != NULL) ? meta->reached_end : 0U,
                (meta != NULL) ? meta->final_total_count : 0U);
    cJSON_Delete(root);
    return 0;
}

/* vlist 查询总条数时会走这里 */
static uint32_t rs_get_count(void *user_ctx)
{
    lv_favorite_song_adapter_t *adapter = (lv_favorite_song_adapter_t *)user_ctx;
    return (adapter != NULL) ? lv_favorite_song_catalog_count(&adapter->catalog) : 0U;
}

/*
 * 请求一个批次。
 * 这里只负责防重：
 * - 已经 ready 的不再请求
 * - 正在 loading 的不再重复请求
 */
static void rs_request_batch(lv_favorite_song_adapter_t *adapter, uint32_t batch_index)
{
    if (adapter == NULL || batch_index >= adapter->batch_count) {
        RS_FLOW_LOG("request batch ignored: adapter=%p batch=%u batch_count=%u", adapter, batch_index, adapter ? adapter->batch_count : 0U);
        return;
    }

    if (adapter->end_reached && batch_index > adapter->last_batch_index) {
        printf("[favorite_song] 跳过请求: batch=%u 已超过最后一页 batch=%u\n",
               batch_index,
               adapter->last_batch_index);
        return;
    }

    if (adapter->batch_ready == NULL || adapter->batch_loading == NULL) {
        RS_FLOW_LOG("request batch state arrays null: batch=%u", batch_index);
        return;
    }

    if (adapter->batch_ready[batch_index] || adapter->batch_loading[batch_index]) {
        RS_HOT_LOG("request batch skip duplicate: batch=%u ready=%u loading=%u",
                   batch_index, adapter->batch_ready[batch_index], adapter->batch_loading[batch_index]);
        return;
    }

    adapter->batch_loading[batch_index] = 1U;
    if (batch_index == 0U) {
        adapter->first_batch_inflight = 1U;
    }
    RS_FLOW_LOG("request batch send: batch=%u epoch=%u", batch_index, adapter->request_epoch);
    fetch_favorite_song_batch((int)batch_index);
}

static int rs_finish_first_batch_gate_if_needed(lv_favorite_song_adapter_t *adapter,
                                                uint32_t batch_index)
{
    uint32_t deferred_total;

    if (adapter == NULL || batch_index != 0U) {
        return 0;
    }

    adapter->first_batch_inflight = 0U;

    if (!adapter->deferred_reset) {
        return 0;
    }

    deferred_total = adapter->deferred_total_count;
    adapter->deferred_reset = 0U;
    adapter->deferred_total_count = 0U;

    printf("[favorite_song] 首批回调完成，补做最后一次reset: total=%u epoch=%u\n",
           deferred_total,
           adapter->request_epoch);

    lv_favorite_song_adapter_reset(adapter, deferred_total);
    return 1;
}

/* vlist 取行数据时调用。这里顺手按需触发对应批次请求。 */
static bool rs_get_item(uint32_t index, lv_vlist_item_t *out, void *user_ctx)
{
    lv_favorite_song_adapter_t *adapter = (lv_favorite_song_adapter_t *)user_ctx;
    uint32_t batch_index;

    if (adapter == NULL || out == NULL) return false;

    batch_index = index / adapter->batch_size;
    rs_request_batch(adapter, batch_index);

    return lv_favorite_song_catalog_get_vlist_item(&adapter->catalog, index, out);
}



/* 行点击处理：加入点歌并切换选中状态 */
static void rs_on_item_click(void *user_ctx,
                             uint32_t item_id,
                             uint32_t bound_index,
                             lv_coord_t rel_x,
                             lv_coord_t rel_y)
{
    lv_favorite_song_adapter_t *adapter = (lv_favorite_song_adapter_t *)user_ctx;
    lv_favorite_song_item_t item;
    int item_changed = 0;

    (void)bound_index;
    (void)rel_y;

    if (adapter == NULL || adapter->view_style == NULL) {
        return;
    }

    if (!lv_favorite_song_catalog_get_item(&adapter->catalog, item_id, &item)) {
        RS_FLOW_LOG("item click miss: item_id=%u", item_id);
        return;
    }
    RS_FLOW_LOG("item click: item_id=%u song_id=%u name=%s artist=%s rel_x=%d rel_y=%d",
                item_id, item.song_id, item.name, item.artist, (int)rel_x, (int)rel_y);
    lv_favorite_song_row_style_t  *s = &adapter->view_style->row_style;

    /*记录一下*/
    extern int g_song_id;
    g_song_id = item.song_id;
    record_last_artist(item.artist);
    record_last_song_name(item.name);

    if (rel_x >= s->idx_x    && rel_x < (s->idx_x + s->idx_w))
    {
    }
    if (rel_x >= s->name_x   && rel_x < (s->name_x + s->name_w))
    {
    }
    if (rel_x >= s->artist_x && rel_x < (s->artist_x + s->artist_w))
    {
    }
    if (rel_x >= s->a_x      && rel_x < (s->a_x + s->a_w))
    {
    }
    if (rel_x >= s->b_x      && rel_x < (s->b_x + s->b_w))
    {
        page_nav_push("screen_102",&guider_ui.screen_102,&guider_ui.screen_102_del,setup_scr_screen_102);
    }
    if (rel_x >= s->c_x      && rel_x < (s->c_x + s->c_w))
    {
        snprintf(item.text_c, sizeof(item.text_c), "8");
        item.color_c = 0xFF0000;   /* 红色 */
        item_changed = 1;
        db_list_pro_thread_send_to_worker(DBP_WORKER_MSG_love_song, 0, 0, item.artist, item.name, item.ai_mv_url); 
    }
    if (rel_x >= s->d_x      && rel_x < (s->d_x + s->d_w))
    {
        page_nav_push("screen_2",&guider_ui.screen_2,&guider_ui.screen_2_del,setup_scr_screen_2);
        ktv_cloud_order_resolved_t resolved;
        char current_json[8192];
        char songinfo_json[16384];
        char *p = NULL;
        int a = 1;
        if(item.color_b == 0xff6500)
        {
            p = item.ai_mv_url;
            a = 0;
        }
        ktv_cloud_order_resolve_first_play_param("16666666666",
                                                "fbe29d501e94250e3442d14979913481",
                                                a,   // 或 1
                                                item.song_id,
                                                p,
                                                &resolved,
                                                current_json,
                                                sizeof(current_json),
                                                songinfo_json,
                                                sizeof(songinfo_json));
        ktv_player_ui_play(&resolved.play_param);
    }
    if (rel_x >= s->e_x      && rel_x < (s->e_x + s->e_w))
    {
        db_list_pro_thread_send_to_worker(DBP_WORKER_MSG_add_song, 0, 0, item.artist, item.name, item.ai_mv_url); 
        snprintf(item.text_e, sizeof(item.text_e), "9");
        item_changed = 1;
    }
    if (rel_x >= s->f_x      && rel_x < (s->f_x + s->f_w))
    {

    }

    if (item_changed != 0)
    {
        (void)lv_favorite_song_catalog_set_item_by_slot(&adapter->catalog, item_id, &item);
        if (adapter->vlist != NULL)
        {
            lv_vlist_notify_item_changed(adapter->vlist, item_id);
        }
    }

}

/* 从 pending 队列里取一个待处理批次。 */
static int rs_pop_one_batch(lv_favorite_song_adapter_t *adapter,
                            uint32_t *batch_index,
                            uint8_t *success,
                            uint32_t *request_epoch)
{
    uint32_t i;

    if (adapter == NULL) return 0;

    pthread_mutex_lock(&adapter->pending_lock);
    for (i = 0U; i < adapter->batch_count; ++i) {
        if (adapter->pending_batch_valid[i]) {
            adapter->pending_batch_valid[i] = 0U;
            *batch_index = i;
            *success = adapter->pending_batch_success[i];
            *request_epoch = (adapter->pending_batch_epoch != NULL)
                           ? adapter->pending_batch_epoch[i]
                           : adapter->request_epoch;
            pthread_mutex_unlock(&adapter->pending_lock);
            return 1;
        }
    }

    adapter->has_pending = 0U;
    pthread_mutex_unlock(&adapter->pending_lock);
    return 0;
}

static void rs_apply_last_page_if_needed(lv_favorite_song_adapter_t *adapter,
                                        uint32_t batch_index,
                                        const rs_batch_sync_meta_t *meta)
{
    uint32_t final_total;
    uint32_t final_batch_count;

    if (adapter == NULL || meta == NULL || !meta->reached_end) {
        return;
    }

    final_total = meta->final_total_count;
    if (final_total > adapter->total_count) {
        final_total = adapter->total_count;
    }

    adapter->end_reached = 1U;
    adapter->last_batch_index = batch_index;

    if (final_total == adapter->total_count) {
        printf("[favorite_song] 已到最后一页: batch=%u, recv=%u, batch_size=%u, final_total=%u\n",
               batch_index,
               meta->data_count,
               adapter->batch_size,
               final_total);
        return;
    }

    adapter->total_count = final_total;
    final_batch_count = (final_total == 0U)
                      ? 0U
                      : ((final_total + adapter->batch_size - 1U) / adapter->batch_size);
    adapter->batch_count = final_batch_count;

    /*
     * 这里必须同步收缩 catalog 总数，否则 vlist 仍会认为后面还有页。
     * 只缩小 total_items，不重新分配，避免已加载内容丢失。
     */
    (void)lv_favorite_song_catalog_truncate(&adapter->catalog, final_total);

    printf("[favorite_song] 已到最后一页: batch=%u, recv=%u, batch_size=%u, final_total=%u, json_total_count=%u%s\n",
           batch_index,
           meta->data_count,
           adapter->batch_size,
           final_total,
           meta->json_total_count,
           meta->json_total_count_valid ? "" : "(json未提供)");

    if (adapter->vlist != NULL) {
        RS_FLOW_LOG("adapter reset reload vlist");
        lv_vlist_reload(adapter->vlist);
    }
}

/*
 * UI 线程定时器：
 * - 取出异步线程塞进来的 pending 回调
 * - 校验 epoch
 * - 读取 json
 * - 标记 batch_ready
 * - 通知 vlist 这一段数据发生变化
 */
static void rs_ui_timer_cb(lv_timer_t *timer)
{
    lv_favorite_song_adapter_t *adapter = (lv_favorite_song_adapter_t *)timer->user_data;
    uint32_t batch_index;
    uint32_t request_epoch;
    uint8_t success;

    if (adapter == NULL || adapter->vlist == NULL) return;

    pthread_mutex_lock(&adapter->pending_lock);
    if (!adapter->has_pending) {
        pthread_mutex_unlock(&adapter->pending_lock);
        return;
    }
    pthread_mutex_unlock(&adapter->pending_lock);

    while (rs_pop_one_batch(adapter, &batch_index, &success, &request_epoch)) {
        uint32_t start, count;
        rs_batch_sync_meta_t meta;

        RS_FLOW_LOG("ui timer pop batch: batch=%u success=%u cb_epoch=%u cur_epoch=%u",
                    batch_index, success, request_epoch, adapter->request_epoch);
        if (batch_index < adapter->batch_count && adapter->batch_loading != NULL) {
            adapter->batch_loading[batch_index] = 0U;
        }

        /*
         * 首批门闩要先释放。
         * 注意：即使这是旧 epoch 的首批回调，也必须先把门闩放开，
         * 否则 reset 合并会一直卡住，新的 batch0 永远发不出去。
         */
        if (rs_finish_first_batch_gate_if_needed(adapter, batch_index)) {
            return;
        }

        /* 旧请求：直接丢弃，不允许污染 reset 后的新页面 */
        if (request_epoch != adapter->request_epoch) {
            RS_FLOW_LOG("ui timer drop stale batch: batch=%u cb_epoch=%u cur_epoch=%u",
                        batch_index, request_epoch, adapter->request_epoch);
            continue;
        }

        if (batch_index >= adapter->batch_count) {
            RS_FLOW_LOG("ui timer drop out-of-range batch: batch=%u batch_count=%u",
                        batch_index, adapter->batch_count);
            continue;
        }

        if (!success) {
            RS_FLOW_LOG("ui timer skip failed batch: batch=%u epoch=%u", batch_index, request_epoch);
            continue;
        }
        if (rs_sync_batch_from_json(adapter, batch_index, request_epoch, &meta) != 0) {
            RS_FLOW_LOG("ui timer sync batch failed: batch=%u epoch=%u", batch_index, request_epoch);
            continue;
        }

        rs_apply_last_page_if_needed(adapter, batch_index, &meta);

        if (batch_index >= adapter->batch_count) {
            continue;
        }

        adapter->batch_ready[batch_index] = 1U;
        start = batch_index * adapter->batch_size;
        count = adapter->batch_size;
        if (start >= adapter->total_count) {
            continue;
        }
        if (start + count > adapter->total_count) {
            count = adapter->total_count - start;
        }
        RS_FLOW_LOG("ui timer notify range changed: batch=%u start=%u count=%u",
                    batch_index, start, count);
        lv_vlist_notify_range_changed(adapter->vlist, start, count);
    }
}

/* 初始化 adapter，并创建 UI 定时器 */
int lv_favorite_song_adapter_start(lv_favorite_song_adapter_t *adapter,
                               const lv_favorite_song_view_style_t *view_style,
                               uint32_t total_count,
                               uint32_t batch_size)
{
    if (adapter == NULL || view_style == NULL || total_count == 0U) {
        return -1;
    }

    memset(adapter, 0, sizeof(*adapter));
    RS_FLOW_LOG("adapter start: total=%u batch_size_in=%u", total_count, batch_size);
    adapter->view_style = view_style;
    adapter->total_count = total_count;
    adapter->batch_size = rs_safe_positive_u32(batch_size, 50U);
    adapter->batch_count = (total_count + adapter->batch_size - 1U) / adapter->batch_size;

    if (!lv_favorite_song_catalog_init(&adapter->catalog, total_count)) {
        return -1;
    }

    adapter->batch_ready = (uint8_t *)calloc(adapter->batch_count, sizeof(uint8_t));
    adapter->batch_loading = (uint8_t *)calloc(adapter->batch_count, sizeof(uint8_t));
    adapter->pending_batch_valid = (uint8_t *)calloc(adapter->batch_count, sizeof(uint8_t));
    adapter->pending_batch_success = (uint8_t *)calloc(adapter->batch_count, sizeof(uint8_t));
    adapter->pending_batch_epoch = (uint32_t *)calloc(adapter->batch_count, sizeof(uint32_t));

    if (adapter->batch_ready == NULL ||
        adapter->batch_loading == NULL ||
        adapter->pending_batch_valid == NULL ||
        adapter->pending_batch_success == NULL ||
        adapter->pending_batch_epoch == NULL) {
        lv_favorite_song_adapter_stop(adapter);
        return -1;
    }

    pthread_mutex_init(&adapter->pending_lock, NULL);
    adapter->ui_timer = lv_timer_create(rs_ui_timer_cb, 50U, adapter);
    adapter->started = 1U;
    adapter->request_epoch = 1U;
    adapter->end_reached = 0U;
    adapter->last_batch_index = (adapter->batch_count > 0U) ? (adapter->batch_count - 1U) : 0U;
    adapter->first_batch_inflight = 0U;
    adapter->deferred_reset = 0U;
    adapter->deferred_total_count = 0U;
    g_favorite_song_default = adapter;
    RS_FLOW_LOG("adapter start ok: total=%u batch_size=%u batch_count=%u epoch=%u",
                adapter->total_count, adapter->batch_size, adapter->batch_count, adapter->request_epoch);
    return 0;
}

/* 释放 adapter 内部资源 */
void lv_favorite_song_adapter_stop(lv_favorite_song_adapter_t *adapter)
{
    if (adapter == NULL) {
        return;
    }

    RS_FLOW_LOG("adapter stop: started=%u epoch=%u", adapter->started, adapter->request_epoch);

    if (adapter->ui_timer != NULL) {
        lv_timer_del(adapter->ui_timer);
        adapter->ui_timer = NULL;
    }

    if (adapter->started) {
        pthread_mutex_destroy(&adapter->pending_lock);
    }

    free(adapter->batch_ready);
    free(adapter->batch_loading);
    free(adapter->pending_batch_valid);
    free(adapter->pending_batch_success);
    free(adapter->pending_batch_epoch);

    adapter->batch_ready = NULL;
    adapter->batch_loading = NULL;
    adapter->pending_batch_valid = NULL;
    adapter->pending_batch_success = NULL;
    adapter->pending_batch_epoch = NULL;

    lv_favorite_song_catalog_deinit(&adapter->catalog);

    if (g_favorite_song_default == adapter) {
        g_favorite_song_default = NULL;
    }

    memset(adapter, 0, sizeof(*adapter));
}

/* 创建并绑定虚拟列表 */
lv_vlist_t *lv_favorite_song_adapter_create_vlist(lv_favorite_song_adapter_t *adapter, lv_obj_t *parent)
{
    lv_vlist_config_t cfg;
    lv_vlist_ops_t ops;

    if (adapter == NULL || parent == NULL || adapter->view_style == NULL) {
        return NULL;
    }

    memset(&cfg, 0, sizeof(cfg));
    cfg.parent = parent;
    cfg.visible_rows = rs_safe_positive_u32(adapter->view_style->visible_rows, 1U);
    cfg.visible_cols = rs_safe_positive_u32(adapter->view_style->visible_cols, 1U);

    /*
     * 严重踩坑警示：
     * 1. overscan/preload 的 0 是合法配置，表示“不做前/后向扩展”。
     * 2. 这里绝对不能再把 0 强行改成 1。
     * 3. 之前正是因为这里把 overscan_rows_front=0 兜回了 1，
     *    导致首屏顶部第 0 行被挤到可视区外，表现为“初始化时第一行不显示”。
     *
     * 结论：
     * - 允许 style 显式传 0。
     * - 真正的顶部 clamp 应由 lv_vlist 内部负责；在它修好之前，
     *   本模块预置样式默认把 overscan_rows_front 设为 0，避免首屏丢行。
     */
    cfg.overscan_rows_front = adapter->view_style->overscan_rows_front;
    cfg.overscan_rows_back = adapter->view_style->overscan_rows_back;
    cfg.preload_before = adapter->view_style->preload_before;
    cfg.preload_after = adapter->view_style->preload_after;
    cfg.viewport_width = adapter->view_style->viewport_width;
    cfg.viewport_height = adapter->view_style->viewport_height;
    cfg.cell_width = adapter->view_style->cell_width;
    cfg.cell_height = adapter->view_style->cell_height;
    cfg.gap_x = adapter->view_style->gap_x;
    cfg.gap_y = adapter->view_style->gap_y;
    cfg.user_ctx = adapter;
    cfg.renderer_ops = &g_lv_renderer_favorite_song_ops;
    cfg.renderer_style = &adapter->view_style->row_style;
    cfg.on_item_click = rs_on_item_click;

    memset(&ops, 0, sizeof(ops));
    ops.get_count = rs_get_count;
    ops.get_item = rs_get_item;

    adapter->vlist = lv_vlist_create(&cfg, &ops);

    /*
     * 首次 open 时也要主动拉第 0 批。
     *
     * 警示：
     * - reset 路径已经会主动请求首批；
     * - 但首次 open 如果完全依赖 vlist 在首帧 bind/get_item 时“被动触发”，
     *   就会出现首屏时序不稳定：轻则先看到空白/Loading，重则必须等用户
     *   滑动或点击后才真正开始首批加载。
     *
     * 所以 create 完 vlist 后，立刻显式请求 batch 0，统一 open/reset 行为。
     */
    if (adapter->vlist != NULL) {
        rs_request_batch(adapter, 0U);
    }

    return adapter->vlist;
}

/*
 * 重置 adapter。
 * 1. request_epoch++，旧请求全部作废
 * 2. 清空旧批次状态
 * 3. reload vlist
 * 4. 立即主动请求第 0 批
 */
void lv_favorite_song_adapter_reset(lv_favorite_song_adapter_t *adapter, uint32_t total_count)
{
    if (adapter == NULL || total_count == 0U) {
        RS_FLOW_LOG("adapter reset ignored: adapter=%p total=%u", adapter, total_count);
        return;
    }

    RS_FLOW_LOG("adapter reset start: old_total=%u new_total=%u old_epoch=%u",
                adapter->total_count, total_count, adapter->request_epoch);

    pthread_mutex_lock(&adapter->pending_lock);
    /* 每次 reset 都进入新的一代，旧请求全部失效 */
    adapter->request_epoch += 1U;
    if (adapter->request_epoch == 0U) {
        adapter->request_epoch = 1U;
    }
    adapter->has_pending = 0U;
    pthread_mutex_unlock(&adapter->pending_lock);

    adapter->total_count = total_count;
    adapter->batch_count = (total_count + adapter->batch_size - 1U) / adapter->batch_size;
    adapter->end_reached = 0U;
    adapter->last_batch_index = (adapter->batch_count > 0U) ? (adapter->batch_count - 1U) : 0U;

    (void)lv_favorite_song_catalog_reset(&adapter->catalog, total_count);

    free(adapter->batch_ready);
    free(adapter->batch_loading);
    free(adapter->pending_batch_valid);
    free(adapter->pending_batch_success);
    free(adapter->pending_batch_epoch);

    adapter->batch_ready = (uint8_t *)calloc(adapter->batch_count, sizeof(uint8_t));
    adapter->batch_loading = (uint8_t *)calloc(adapter->batch_count, sizeof(uint8_t));
    adapter->pending_batch_valid = (uint8_t *)calloc(adapter->batch_count, sizeof(uint8_t));
    adapter->pending_batch_success = (uint8_t *)calloc(adapter->batch_count, sizeof(uint8_t));
    adapter->pending_batch_epoch = (uint32_t *)calloc(adapter->batch_count, sizeof(uint32_t));

    if (adapter->vlist != NULL) {
        RS_FLOW_LOG("adapter reset reload vlist");
        lv_vlist_reload(adapter->vlist);
    }

    if (adapter->batch_ready == NULL ||
        adapter->batch_loading == NULL ||
        adapter->pending_batch_valid == NULL ||
        adapter->pending_batch_success == NULL ||
        adapter->pending_batch_epoch == NULL) {
        return;
    }

    /*
     * 最小止血：
     * - 如果首批还在飞，不继续叠新的 batch0 任务；
     * - 只记住最后一次 reset；
     * - 等当前首批回调回来后，再补做最后一次 reset。
     */
    if (adapter->first_batch_inflight) {
        adapter->deferred_reset = 1U;
        adapter->deferred_total_count = total_count;
        printf("[favorite_song] reset合并: 首批仍在飞，延后到首批回调后再做 total=%u epoch=%u\n",
               total_count,
               adapter->request_epoch);
        return;
    }

    /* reset 后立即主动拉首批，避免用户还要再点一下 */
    RS_FLOW_LOG("adapter reset request first batch: epoch=%u", adapter->request_epoch);
    rs_request_batch(adapter, 0U);
}

/* 业务层读取某一行完整数据 */
bool lv_favorite_song_adapter_get_business_item(lv_favorite_song_adapter_t *adapter, uint32_t item_id, lv_favorite_song_item_t *out)
{
    if (adapter == NULL) {
        return false;
    }
    return lv_favorite_song_catalog_get_item(&adapter->catalog, item_id, out);
}

/* 切换选中状态并刷新单行 */
void lv_favorite_song_adapter_toggle_selected(lv_favorite_song_adapter_t *adapter, uint32_t item_id)
{
    lv_favorite_song_item_t item;

    if (adapter == NULL) {
        return;
    }

    if (!lv_favorite_song_catalog_get_item(&adapter->catalog, item_id, &item)) {
        return;
    }

    lv_favorite_song_catalog_set_selected(&adapter->catalog, item_id, !item.selected);
    if (adapter->vlist != NULL) {
        lv_vlist_notify_item_changed(adapter->vlist, item_id);
    }
}

/* 兼容旧接口。新代码优先使用带 epoch 版本。 */
void lv_favorite_song_adapter_notify_batch_ready(lv_favorite_song_adapter_t *adapter, uint32_t batch_index, bool success)
{
    if (adapter == NULL) {
        return;
    }

    lv_favorite_song_adapter_notify_batch_ready_with_epoch(adapter,
                                                       batch_index,
                                                       success,
                                                       adapter->request_epoch);
}

/* 异步线程把结果塞进 pending 区，这里不直接刷新 UI。 */
void lv_favorite_song_adapter_notify_batch_ready_with_epoch(lv_favorite_song_adapter_t *adapter,
                                                        uint32_t batch_index,
                                                        bool success,
                                                        uint32_t request_epoch)
{
    if (adapter == NULL || batch_index >= adapter->batch_count) {
        RS_FLOW_LOG("notify batch ignored: adapter=%p batch=%u batch_count=%u", adapter, batch_index, adapter ? adapter->batch_count : 0U);
        return;
    }

    RS_FLOW_LOG("notify batch ready: batch=%u success=%u epoch=%u", batch_index, success ? 1U : 0U, request_epoch);
    pthread_mutex_lock(&adapter->pending_lock);
    adapter->pending_batch_valid[batch_index] = 1U;
    adapter->pending_batch_success[batch_index] = success ? 1U : 0U;
    if (adapter->pending_batch_epoch != NULL) {
        adapter->pending_batch_epoch[batch_index] = request_epoch;
    }
    adapter->has_pending = 1U;
    pthread_mutex_unlock(&adapter->pending_lock);
}

/* 取默认 adapter */
lv_favorite_song_adapter_t *lv_favorite_song_adapter_get_default(void)
{
    return g_favorite_song_default;
}

/* 读取当前代号 */
uint32_t lv_favorite_song_adapter_get_request_epoch(const lv_favorite_song_adapter_t *adapter)
{
    return (adapter != NULL) ? adapter->request_epoch : 0U;
}

/* 默认 adapter 的当前代号 */
uint32_t lv_favorite_song_adapter_get_default_request_epoch(void)
{
    return lv_favorite_song_adapter_get_request_epoch(g_favorite_song_default);
}

/* 默认 adapter 的旧接口快捷方式 */
void lv_favorite_song_adapter_notify_default_batch_ready(uint32_t batch_index, bool success)
{
    lv_favorite_song_adapter_notify_default_batch_ready_with_epoch(batch_index,
                                                               success,
                                                               lv_favorite_song_adapter_get_default_request_epoch());
}

/* 默认 adapter 的带 epoch 快捷方式 */
void lv_favorite_song_adapter_notify_default_batch_ready_with_epoch(uint32_t batch_index,
                                                                bool success,
                                                                uint32_t request_epoch)
{
    lv_favorite_song_adapter_notify_batch_ready_with_epoch(g_favorite_song_default,
                                                       batch_index,
                                                       success,
                                                       request_epoch);
}
