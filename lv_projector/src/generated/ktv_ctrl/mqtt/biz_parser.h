#ifndef BIZ_PARSER_H
#define BIZ_PARSER_H

// 增加 topic 参数，供网络层传入消息来源
void biz_parse(const char* topic, const char* json_str);

#endif
