#include "file_io.h"

// 读取文件内容
char* File_Read(const char* path, int *fileLen)
{
    char *file = NULL;
    FILE *pFile = NULL;
    struct stat fp_info;

    if (fileLen) *fileLen = 0;

    if (stat(path, &fp_info) != 0)
    {
        printf("文件不存在: %s\n", path);
        return NULL;
    }

    pFile = fopen(path, "rb");
    if (pFile == NULL)
    {
        printf("文件打开失败: %s\n", path);
        return NULL;
    }

    fseek(pFile, 0, SEEK_END);
    long len = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);
    if (fileLen) *fileLen = (int)len;

    file = (char *)malloc(len + 1);
    if (file == NULL)
    {
        printf("内存分配失败\n");
        fclose(pFile);
        return NULL;
    }
    memset(file, 0, len + 1);

    fread(file, 1, len, pFile);
    fclose(pFile);

    return file;
}

// 写入数据到文件，支持覆盖或追加
// append 参数：0 为覆盖，1 为追加
int File_Write(const char* path, const char* data, int dataLen, int append)
{
    FILE *pFile = NULL;
    const char* mode = append ? "ab" : "wb";  // 追加模式 "ab"，写入模式 "wb"
    
    pFile = fopen(path, mode);
    if (pFile == NULL)
    {
        printf("文件打开失败: %s\n", path);
        return -1;
    }

    size_t written = fwrite(data, 1, dataLen, pFile);
    if (written != dataLen)
    {
        printf("文件写入失败: %s\n", path);
        fclose(pFile);
        return -1;
    }

    fclose(pFile);
    return 0; // 成功
}
