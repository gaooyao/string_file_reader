/**********************************************************************
*
* 文件名称：string_file_reader.h
* 内容摘要：文本文件读取插件
* 作    者：gaooyao
* 邮    箱：1625545050@qq.com
* 项目地址：https://github.com/gaooyao/string_file_reader
* 简    介：通过引入缓冲区，减少读取超大文本时访问外存的频率，实现了文件的加速读取，读写性能得到明显提升。
* 使用方法：1：创建FileHandler指针
*          2：调用open_file函数，打开文件
*          3：调用read_line或者write_line函数进行文件读写
*          4：使用文件结束，调用close_file函数关闭文件
*
**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "string_file_reader.h"

int FILE_BUFFER_SIZE = 262144; //文件缓冲区大小，单位字节

/*
* 函数名称：open_file
* 函数功能：根据给定属性创建新的FileHandler对象
* 参数：    file_name：要打开的文件名
*          open_type:文件打开类型，如"r","w"
* 返回值：FileHandler指针
*/
FileHandler *open_file(char *file_name, char *open_type)
{
    if (strcmp(open_type, "rb") && strcmp(open_type, "wb"))
    {
        return 0; //如果文件打开模式错误，则终止打开文件
    }
    /* 创建FileHandler */
    FileHandler *file_handler = (FileHandler *)malloc(sizeof(FileHandler));
    if (!file_handler)
    {
        printf("open %s fail!", file_name);
        return NULL;
    }
    /* 打开文件 */
    FILE *_file = (FILE *)malloc(sizeof(FILE));
    _file = fopen((const char *)file_name, open_type);
    if (!_file)
    {
        printf("open %s fail!", file_name);
        return NULL;
    }
    file_handler->file = _file;
    /* 创建文件缓冲buffer */
    file_handler->buffer_size = FILE_BUFFER_SIZE;
    file_handler->buffer = (char *)malloc(file_handler->buffer_size);
    if (!file_handler->buffer)
    {
        fclose(file_handler->file);
        return 0; //分配内存失败
    }
    /* FileHandler属性初始化 */
    strcpy(file_handler->file_name, file_name);
    file_handler->open_status = (!strcmp(open_type, "rb") ? 1 : 2); //三元表达式
    file_handler->point = -1;
    return file_handler;
}

/*
* 函数名称：read_line
* 函数功能：从文件中读取一行文本
* 参数：	file_handler：被读取的FileHandler对象
*		    str：存放被读取文本的地址
* 返回值：int，1为读取成功，0为读取失败
*/
int read_line(FileHandler *file_handler, char **str)
{
    if (file_handler->open_status != 1)
    {
        return 0; //打开模式为写入时不允许读取
    }
    int old_point = file_handler->point; //记录当前指针位置
    if (file_handler->point != -1 && *(file_handler->buffer + file_handler->point) == -1)
    {
        return 0; //如果已读到文件末尾则直接返回
    }
    do
    {
        if (*(file_handler->buffer + file_handler->point) == -1) //遍历时如果遇到文件结束符则返回最后一行的开始指针
        {
            (*str) = (file_handler->buffer + old_point + 1);
            return 1;
        }
        //根据当前指针判断是否需要读入新数据
        int is_read_new_data = 0;
        //如果buffer为空，则读入新buffer
        if (file_handler->point == -1)
        {
            int read_num;
            read_num = fread(file_handler->buffer, 1, file_handler->buffer_size, file_handler->file);
            if (!read_num)
            {
                return 0;
            }
            else
            {
                if (read_num < file_handler->buffer_size)
                {
                    *(file_handler->buffer + read_num) = -1; //如果读入的数据是整个文件的最后一块，则读入的数据最后面加一个文件结束符
                }
                is_read_new_data = 1;
            }
        }
        //如果buffer内不足一行，则读入新数据
        if (file_handler->point + 1 >= file_handler->buffer_size)
        {
            //copy buffer尾部数据至头部
            int copy_num = 0;
            old_point++;
            while (old_point < file_handler->buffer_size)
            {
                *(file_handler->buffer + copy_num) = *(file_handler->buffer + old_point);
                old_point++;
                copy_num++;
            }
            //读入新数据
            int read_num;
            read_num = fread((char *)(file_handler->buffer + copy_num), 1, file_handler->buffer_size - copy_num, file_handler->file);
            if (!read_num)
            {
                return 0;
            }
            else
            {
                if (copy_num + read_num < file_handler->buffer_size)
                {
                    *(file_handler->buffer + copy_num + read_num) = -1; //如果读入的数据是整个文件的最后一块，则读入的数据最后面加一个文件结束符
                }
                is_read_new_data = 1;
            }
        }
        //若读入新数据则重置buffer指针
        if (is_read_new_data)
        {
            file_handler->point = 0;
            //判断读入的新数据是否可用
            while (*(file_handler->buffer + file_handler->point) != 10 && *(file_handler->buffer + file_handler->point) != 0 && *(file_handler->buffer + file_handler->point) != -1)
            {
                file_handler->point++;
                if (file_handler->point > file_handler->buffer_size)
                {
                    //读取到的整个buffer内容少于一行，则超过最大处理大小//-1结束//10换行符
                    system("stop");
                    return 0;
                }
            }
            //数据读取成功，str指针指向字符串开头并返回
            *(file_handler->buffer + file_handler->point) = 0; //断行
            (*str) = &(file_handler->buffer[0]);
            return 1;
        }
        file_handler->point++;
    } while (*(file_handler->buffer + file_handler->point) != '\n'); //point指针循环后移，直到遇到换行符则停止
    //没有读入新数据，但已找到被读文本的开头和结尾
    *(file_handler->buffer + file_handler->point) = 0; //断行
    (*str) = (file_handler->buffer + old_point + 1);
    return 1;
}

/*
* 函数名称：write_line
* 函数功能：写入文件
* 参数：	file_handler：被写入的FileHandler对象
            str：被写入的字符串
* 返回值：int,是否写入成功
*/
int write_line(FileHandler *file_handler, char *str)
{
    if (file_handler->open_status != 2)
    {
        return 0; //如果文件为读模式则不允许写入
    }
    while (*str != 0 && *str != -1) //每次在buffer中写入一字节，直到遇到结束符
    {
        file_handler->point++;
        if (file_handler->point >= file_handler->buffer_size) //如果buffer已满，则把当前buffer缓存数据写入文件，然后从buffer开始位置继续写入
        {
            fwrite(file_handler->buffer, file_handler->buffer_size, 1, file_handler->file);
            file_handler->point = 0;
        }
        *(file_handler->buffer + file_handler->point) = *str;
        str++;
    }
    file_handler->point++;
    if (file_handler->point >= file_handler->buffer_size) //buffer中额外写入一个换行符，如果buffer已满，则把当前buffer缓存数据写入文件，然后从buffer开始位置继续写入
    {
        fwrite(file_handler->buffer, file_handler->buffer_size, 1, file_handler->file);
        file_handler->point = 0;
    }
    *(file_handler->buffer + file_handler->point) = 10;
    return 0;
}

/*
* 函数名称：reset_file
* 函数功能：重置FileHandler对象，返回到文件开头
* 参数：	file_handler：被重置的FileHandler对象
* 返回值：int,是否重置成功
*/
int reset_file(FileHandler *file_handler)
{
    if (file_handler->open_status != 1)
        return 0; //打开模式为写入时不允许回到文件开头
    file_handler->point = -1;
    rewind(file_handler->file);
    return 1;
}

/*r
* 函数名称：close_file
* 函数功能：关闭文件，释放FileHandler对象
* 参数：	file_handler：被关闭的FileHandler对象
* 返回值：int,是否关闭成功
*/
int close_file(FileHandler *file_handler)
{
    //若文件为写入方式打开，则把缓冲区内容写入文件
    if (file_handler->open_status == 2 && file_handler->point != -1)
    {
        fwrite(file_handler->buffer, file_handler->point, 1, file_handler->file);
    }
    //关闭文件并释放空间
    fclose(file_handler->file);
    free(file_handler->buffer);
    file_handler->open_status = 0;
    free(file_handler);
    return 1;
}
