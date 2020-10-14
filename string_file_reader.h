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

#ifndef string_file_reader
#define string_file_reader

#include <stdio.h>

typedef struct FileHandler
{							  //文件handler
	FILE *file;				  //原始文件操作handler
	char file_name[256];	  //被打开的文件名
	int open_status;		  //文件打开状态，0未打开，1读模式，2写模式
	int point;				  //当前在buffer中的操作位置指针
	unsigned int buffer_size; //缓冲区大小
	char *buffer;			  //缓冲区
} FileHandler;

FileHandler *open_file(char *file_name, char *open_type); //打开新文件，open_type为"rb"或"wb"
int read_line(FileHandler *file_handler, char **str);	  //从文件中读取一行
int write_line(FileHandler *file_handler, char *str);	  //写入一行
int reset_file(FileHandler *file_handler);				  //对于读模式，此函数可回到文件开头，下次read_line时返回文件第一行内容
int close_file(FileHandler *file_handler);				  //关闭文件

#endif
