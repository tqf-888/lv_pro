#ifndef PAGE_MANAGER_H
#define PAGE_MANAGER_H


/**
 * @brief 初始化/重置所有页面管理变量
 * 
 * 将页面ID、分级ID、歌手ID重置为 IGNORE_NUM，
 * 名称字符串重置为空字符串。
 */
void page_manager_init(void);

void page_set(int page);
int page_get(void);

void subpage_set(int sub);
int subpage_get(void);

void singer_id_set(int singer_id);
int singer_id_get(void);

void name_set(const char *name);
const char* name_get(void);

#endif /* PAGE_MANAGER_H */