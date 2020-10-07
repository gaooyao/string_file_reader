/**********************************************************************
*
* 文件名称：test.c
* 内容摘要：文本文件读取插件使用方法Demo
* 作    者：gaooyao
* 邮    箱：1625545050@qq.com
*
**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "string_file_reader.h"

int main()
{
    char *str = NULL;
    FileHandler *file_a;
    FileHandler *file_b;
    clock_t start, end;
    int i = 0;
    file_a = open_file("test_input.txt", "rb");
    file_b = open_file("test_output.txt", "wb");
    start = clock();
    while (read_line(file_a, &str))
    {
        i++;
        write_line(file_b, str);
    }
    end = clock();
    close_file(file_a);
    close_file(file_b);
    printf("文件复制完成，共%d行,用时%f秒。请打开output.txt查看结果。\n", i, ((float)end - start) / 1000);
    system("pause");
    return 0;
}