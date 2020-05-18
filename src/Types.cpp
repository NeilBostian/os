#include <Types.h>

uint32 String::Len(string str)
{
    int32 ret = 0;
    while (str[ret] != 0)
        ret++;
    return ret;
}

void String::ConvertToHex(uint8 x, char *res)
{
    char as32[9];
    String::ConvertToHex((uint32)x, as32);
    res[0] = as32[6];
    res[1] = as32[7];
    res[2] = '\0';
}

void String::ConvertToBin(uint8 x, char *res)
{
    int i = 0;
    for (; i < 8; i++)
    {
        uint8 mask = 1 << i;
        if (x & mask)
        {
            res[7 - i] = '1';
        }
        else
        {
            res[7 - i] = '0';
        }
    }
    res[8] = '\0';
}

void String::ConvertToHex(uint32 x, char *res)
{
    int i = 0;
    for (; i < 8; i++)
    {
        char nybble = (x & (0xF << 4 * (8 - i - 1))) >> 4 * (8 - i - 1);
        if (nybble <= 9)
        {
            res[i] = '0' + nybble;
        }
        else
        {
            res[i] = 'A' + nybble - 10;
        }
    }
    res[8] = '\0';
}
