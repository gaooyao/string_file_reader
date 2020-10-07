# 文本文件读取插件
通过引入缓冲区，减少读取超大文本时访问外存的频率，实现了文件的加速读取，读写性能得到明显提升。

使用方法：
1. 创建FileHandler指针
2. 调用open_file函数，打开文件
3. 调用read_line或者write_line函数进行文件读写
4. 使用文件结束，调用close_file函数关闭文件

具体Demo请参阅test.c。
