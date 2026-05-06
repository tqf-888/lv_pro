#ifndef FILE_IO_H
#define FILE_IO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// 读取文件内容
char* File_Read(const char* path, int *fileLen);

// 写入数据到文件，支持覆盖或追加
int File_Write(const char* path, const char* data, int dataLen, int append);

#endif // FILE_IO_H
