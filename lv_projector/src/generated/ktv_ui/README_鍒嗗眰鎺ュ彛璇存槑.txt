这版改成了“业务层常驻 + 页面层刷新”的结构，拒绝把 add/delete 写死在当前页面里。

一、分层
1) store 层（全局内存仓库，和页面解耦）
   - favorite_list/store/lv_favorite_store.h
   - sung_history/store/lv_sung_history_store.h

2) adapter 层（纯显示）
   - *_adapter_refresh_from_store()
   - *_adapter_delete_item_from_store_and_refresh()

3) app/page 层（给页面入口和按钮回调用）
   - *_page_demo_refresh()
   - *_page_demo_delete_item_and_refresh()

二、添加接口（任意页面、任意时刻可调）
收藏：
    lv_favorite_store_record_t rec;
    memset(&rec, 0, sizeof(rec));
    rec.record_id = 1001;
    rec.media_id  = 5892764;
    rec.is_vip    = 1;
    rec.has_mv    = 0;
    rec.use_default_cdef = 1;
    snprintf(rec.title, sizeof(rec.title), "%s", "渔家姑娘在海边");
    snprintf(rec.artist, sizeof(rec.artist), "%s", "梦之旅合唱组合");
    lv_favorite_store_add(&rec);

唱过：
    lv_sung_history_store_record_t rec;
    memset(&rec, 0, sizeof(rec));
    rec.record_id = 2001;
    rec.media_id  = 876;
    rec.is_vip    = 1;
    rec.has_mv    = 1;
    rec.use_default_cdef = 1;
    snprintf(rec.title, sizeof(rec.title), "%s", "沧海一声笑");
    snprintf(rec.artist, sizeof(rec.artist), "%s", "任贤齐");
    lv_sung_history_store_add(&rec);

三、进入页面时刷新
收藏页面 open 后已经自动调用：
    lv_favorite_list_adapter_refresh_from_store(&page->adapter);

也可以手动：
    favorite_list_page_demo_refresh(page);

唱过同理：
    sung_history_page_demo_refresh(page);

四、删除接口
业务层删：
    lv_favorite_store_delete_by_record_id(id);
    lv_favorite_store_delete_by_media_id(media_id);
    lv_favorite_store_delete_at(index);

页面层删并刷新：
    favorite_list_page_demo_delete_item_and_refresh(page, item_id);
    sung_history_page_demo_delete_item_and_refresh(page, item_id);

五、按钮回调建议
按钮点击时，不要直接改 renderer；在按钮回调里调 page 层接口：
    favorite_list_page_demo_delete_item_and_refresh(page, item_id);

这样业务、显示、页面控制是分开的，不耦合。
