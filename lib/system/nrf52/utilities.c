/**
 * Utilities.
 *
 */

#include "utilities.h"

/*!
 * Redefinition of rand() and srand() standard C functions.
 * These functions are redefined in order to get the same behavior across
 * different compiler toolchains implementations.
 */
// Standard random functions redefinition start
#define RAND_LOCAL_MAX 2147483647L

static uint32_t next = 1;

int32_t rand1( void )
{
    return ( ( next = next * 1103515245L + 12345L ) % RAND_LOCAL_MAX );
}

void srand1( uint32_t seed )
{
    next = seed;
}
// Standard random functions redefinition end

int32_t randr( int32_t min, int32_t max )
{
    return ( int32_t )rand1( ) % ( max - min + 1 ) + min;
}

void memcpy1( uint8_t *dst, const uint8_t *src, uint16_t size )
{
    while( size-- )
    {
        *dst++ = *src++;
    }
}

void memcpyr( uint8_t *dst, const uint8_t *src, uint16_t size )
{
    dst = dst + ( size - 1 );
    while( size-- )
    {
        *dst-- = *src++;
    }
}

void memset1( uint8_t *dst, uint8_t value, uint16_t size )
{
    while( size-- )
    {
        *dst++ = value;
    }
}

int8_t Nibble2HexChar( uint8_t a )
{
    if( a < 10 )
    {
        return '0' + a;
    }
    else if( a < 16 )
    {
        return 'A' + ( a - 10 );
    }
    else
    {
        return '?';
    }
}

void LittleEndian_Uint32ToHex( uint32_t num, uint8_t *hex )
{
    hex[0] = num & 0xFF;
    hex[1] = (num >> 8) & 0xFF;
    hex[2] = (num >> 16) & 0xFF;
    hex[3] = (num >> 24) & 0xFF;
}

void BigEndian_Uint32ToHex( uint32_t num, uint8_t *hex )
{
    hex[3] = num & 0xFF;
    hex[2] = (num >> 8) & 0xFF;
    hex[1] = (num >> 16) & 0xFF;
    hex[0] = (num >> 24) & 0xFF;
}

uint8_t BCC_Check( uint8_t *data, uint32_t len )
{
    uint8_t bcc = 0;
    if( data != NULL )
    {
        for( int i = 0; i < len; i++ )
        {
            bcc ^= data[i];
        }
    }
    return bcc;
}

void HexToStr(uint8_t *pbStr, uint8_t *pbHex, uint16_t h_Len)
{
    uint8_t ddl,ddh;
    uint16_t i;

    for (i = 0; i < h_Len; i++)
    {
        ddh = 48 + pbHex[i] / 16;
        ddl = 48 + pbHex[i] % 16;
        if (ddh > 57) ddh = ddh + 7;
        if (ddl > 57) ddl = ddl + 7;
        pbStr[i*2] = ddh;
        pbStr[i*2+1] = ddl;
    }

    pbStr[h_Len*2] = '\0';
}

void StrToHex(uint8_t *pbHex, uint8_t *pbStr, uint16_t h_Len)
{
    uint8_t h1,h2;
    uint8_t s1,s2;
    uint16_t i;

    for (i = 0; i < h_Len; i++)
    {
        h1 = pbStr[2*i];
        h2 = pbStr[2*i+1];

        s1 = toupper(h1) - 0x30;
        if (s1 > 9)
            s1 -= 7;

        s2 = toupper(h2) - 0x30;
        if (s2 > 9)
            s2 -= 7;

        pbHex[i] = s1*16 + s2;
    }
}

// /*
// CRC-8       x8+x5+x4+1              0x31（0x131）
// CRC-8       x8+x2+x1+1              0x07（0x107）
// CRC-8       x8+x6+x4+x3+x2+x1       0x5E（0x15E）
// */
// uint8_t crc8_compute(uint8_t *rec_values, uint16_t len)
// {
//     uint8_t crc = 0x00;
//     // uint16_t current_byte;
//     uint8_t bit;
//     // for (current_byte = 0; current_byte < len; current_byte++)
//     while(len--)
//     {
//         // crc ^= (rec_values[current_byte]);
//         crc ^= *rec_values++;
//         for (bit = 8; bit > 0; --bit)
//         {
//             if (crc & 0x80)
//             {
//                 crc = (crc << 1) ^ 0x07;
//             }
//             else
//             {
//                 crc = (crc << 1);
//             }
//         }
//     }
//     return crc;
// }

/*
CRC-8       x8+x5+x4+1              0x31（0x131）
CRC-8       x8+x2+x1+1              0x07（0x107）
CRC-8       x8+x6+x4+x3+x2+x1       0x5E（0x15E）
*/
uint8_t crc8_compute(uint8_t *rec_values, uint16_t len)
{
    uint8_t crc = 0x00;
    // uint16_t current_byte;
    uint8_t bit;
    // for (current_byte = 0; current_byte < len; current_byte++)
    while(len--)
    {
        // crc ^= (rec_values[current_byte]);
        crc ^= *rec_values++;
        for (bit = 8; bit > 0; --bit)
        {
            if (crc & 0x01)
            {
                crc = (crc >> 1) ^ 0x8C;
            }
            else
            {
                crc = (crc >> 1);
            }
        }
    }
    return crc;
}

uint16_t crc16_modbus(uint8_t *ptr, uint16_t len)
{
    unsigned char i;
    unsigned short crc = 0xFFFF;
    if (len == 0) {
        len = 1;
    }
    while (len--) {
        crc ^= *ptr;
        for (i = 0; i<8; i++)
        {
            if (crc & 1) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
        ptr++;
    }
    return(crc);
}
