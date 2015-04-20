//公用的头文件和定义文件
//public.c

#include "public.h"
#include "stdio.h"



bool chartoint(char* str, int* result)
{
    int sign = 1;//记录第一位是否是负号
    *result = 0;

    assert(str!=NULL);

    if(*str == '-')
    {
        sign = -1;
        str += 1;
    }

    while(*str!='\0')
    {
        if(*str<'0' || *str>'9')
            return false;

        *result = *result * 10 + *str - '0';
        ++str;
    }

    *result *= sign;
    return true;
}






