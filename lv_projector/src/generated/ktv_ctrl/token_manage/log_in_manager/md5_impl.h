#ifndef MD5_IMPL_H
#define MD5_IMPL_H

#include <stddef.h> // for size_t

// 避免与 OpenSSL 冲突，使用独立命名空间
#define MY_MD5_DIGEST_LEN 16
#define MY_MD5_HEX_LEN    32

typedef struct {
    unsigned int count[2];
    unsigned int state[4];
    unsigned char buffer[64];
} my_md5_ctx_t;

// 初始化上下文
void my_md5_init(my_md5_ctx_t *ctx);

// 更新数据
void my_md5_update(my_md5_ctx_t *ctx, const unsigned char *input, size_t input_len);

// 最终计算
void my_md5_final(my_md5_ctx_t *ctx, unsigned char digest[MY_MD5_DIGEST_LEN]);

// 便捷函数：直接计算字符串的十六进制 MD5
// input: 输入字符串
// output: 输出缓冲区 (需至少 33 字节)
void my_md5_hex_string(const char *input, char *output);

#endif /* MD5_IMPL_H */
