/*
 * File      : ap_math.c
 *
 * Change Logs:
 * Date           Author       Notes
 * 2016-07-01     zoujiachi    first version.
 */
 
#include <rthw.h>
#include <rtdevice.h>
#include <rtthread.h>
#include <math.h>

// 快速算“平方根的倒数”。
// http://zh.wikipedia.org/wiki/%E5%B9%B3%E6%96%B9%E6%A0%B9%E5%80%92%E6%95%B0%E9%80%9F%E7%AE%97%E6%B3%95
float math_rsqrt(float number)
{
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y  = number;
    i  = * ( long * ) &y;                       // evil floating point bit level hacking（对浮点数的邪恶位级hack）
    i  = 0x5f3759df - ( i >> 1 );               // what the fuck?（这他妈的是怎么回事？）
    y  = * ( float * ) &i;
    y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration （第一次牛顿迭代）
    y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed（第二次迭代，可以删除）

    return y;
}

void math_vector_cross(float result[3],const float left[3],const float right[3])
{
    result[0] = left[1]*right[2] - left[2]*right[1];
    result[1] = left[2]*right[0] - left[0]*right[2];
    result[2] = left[0]*right[1] - left[1]*right[0];
}

float math_vector_dot(const float left[3],const float right[3])
{
    return left[0]*right[0] + left[1]*right[1] + left[2]*right[2];
}

float math_vector_length(const float v[3])
{
    return sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

// 计算CRC-16-CCITT。
// http://www.dzjs.net/html/qianrushixitong/2007/0530/2162.html
// crc为上次的结果，开始时设为0。用于分段计算。
uint16_t math_crc16(uint16_t crc,const void * data,uint16_t len)
{
    const static uint16_t crc_tab[16] =
    {
        0x0000 , 0x1021 , 0x2042 , 0x3063 , 0x4084 , 0x50A5 , 0x60C6 , 0x70E7 ,
        0x8108 , 0x9129 , 0xA14A , 0xB16B , 0xC18C , 0xD1AD , 0xE1CE , 0xF1EF
    };
    uint8_t h_crc;
    const uint8_t * ptr = (const uint8_t *)data;
    //
    while(len --)
    {
        h_crc = (uint8_t)(crc >> 12);
        crc <<= 4;
        crc ^= crc_tab[h_crc ^ ((*ptr) >> 4)];
        //
        h_crc = crc >> 12;
        crc <<= 4;
        crc ^= crc_tab[h_crc ^ ((*ptr) & 0x0F)];
        //
        ptr ++;
    }
    //
    return crc;
}

// 整数转字符串。
void math_itoa(int32_t val,char * str)
{
    char buf[16];
    buf[15] = '\0';
    uint8_t index = 16;
    int nagative = 0;
    //
    if(val == 0)
    {
        // 先考虑val=0的情况。
        str[0] = '0';
        str[1] = '\0';
        return;
    }
    //
    // 考虑val<0的情况。
    if(val < 0)
    {
        val = -val;
        nagative = 1;
    }
    //
    while(val > 0 && index != 0)
    {
        index--;
        buf[index] = val % 10 + '0';
        val /= 10;
    }
    //
    // 复制结果。
    int i_str = 0;
    if(nagative)
        str[i_str++] = '-';
    for(int i_buf=index;i_buf<16;i_buf++)
        str[i_str++] = buf[i_buf];
    str[i_str++] = '\0';
    //
    return;
}

// 整数转字符串。
// 不可重入，内存空间由函数管理。
const char * math_afromi(int32_t val)
{
    // int32_t最多只有10位，16字节足够。
    static char buffer[16]; 
    //
    math_itoa(val,buffer);
    //
    return buffer;
}
