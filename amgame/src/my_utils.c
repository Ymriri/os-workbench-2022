#include "my_utils.h"

void my_send_char(const char *s)
{
    putch(*s);
}

int Print(const char *str, ...)
{
    if (str == NULL)
        return -1;

    unsigned int ret_num = 0; // 返回打印字符的个数
    char *pStr = (char *)str; // 指向str
    int ArgIntVal = 0;        // 接收整型
    // unsigned long ArgHexVal = 0; // 接十六进制
    char *ArgStrVal = NULL; // 接收字符型
    // double ArgFloVal = 0.0;      // 接受浮点型
    unsigned long val_seg = 0; // 数据切分
    // unsigned long val_temp = 0; // 临时保存数据
    // int输出使用
    char int_char[12];
    int int_char_count = 0;
    char temp;
    char TEMP_CHAR = ' ';
    va_list pArgs;        // 定义va_list类型指针，用于存储参数的地址
    va_start(pArgs, str); // 初始化pArgs
    while (*pStr != '\0')
    {
        switch (*pStr)
        {
        case ' ':
            my_send_char(pStr);
            ret_num++;
            break;
        case '\t':
            my_send_char(pStr);
            ret_num += 4;
            break;
        case '\r':
            my_send_char(pStr);
            ret_num++;
            break;
        case '\n':
            my_send_char(pStr);
            ret_num++;
            break;
        case '%':
            pStr++;
            // % 格式解析
            switch (*pStr)
            {
            case '%':
                putch('%'); // %%，输出%
                ret_num++;
                pStr++;
                continue;
            case 'c':
                ArgIntVal = va_arg(pArgs, int); // %c，输出char
                temp = (char)ArgIntVal;
                my_send_char(&temp);
                ret_num++;
                pStr++;
                continue;
            case 'd':
                // 接收整型
                ArgIntVal = va_arg(pArgs, int);
                if (ArgIntVal < 0) // 如果为负数打印，负号
                {
                    ArgIntVal = -ArgIntVal; // 取相反数
                    putch('-');
                    ret_num++;
                }
                val_seg = ArgIntVal; // 赋值给 val_seg处理数据
                int_char_count = 0;
                // int转成char类型字符
                while (val_seg > 0)
                {
                    int_char[int_char_count++] = (char)(val_seg % 10 + 48);
                    val_seg /= 10;
                }
                int_char_count--;
                while (int_char_count >= 0)
                {
                    my_send_char(&int_char[int_char_count--]);
                }
                // 计算ArgIntVal长度
                pStr++;
                continue;
            case 'o':
                // 接收整型
                puts("Error,Not at this time!");
                continue;
            case 'x':
                // 接收16进制
                puts("Error,Not at this time!");
                continue;
            case 'b':
                // 接收整型
                puts("Error,Not at this time!");
                continue;
            case 's':
                // 接收字符
                ArgStrVal = va_arg(pArgs, char *);

                while (ArgStrVal)
                {
                    my_send_char(ArgStrVal);
                    ArgStrVal++;
                }

                pStr++;
                continue;

            case 'f':
                // 接收浮点型 保留6为小数，不采取四舍五入

                puts("Error,Not at this time!");
                continue;
            default: // % 匹配错误，暂输出空格
                my_send_char(&TEMP_CHAR);
                ret_num++;
                continue;
            }

        default:
            my_send_char(pStr);
            ret_num++;
            break;
        }
        pStr++;
    }
    va_end(pArgs); // 结束取参数

    return ret_num;
}
