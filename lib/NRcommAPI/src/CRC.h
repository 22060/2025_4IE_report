/**
 * @file CRC.h
 * @author  K.Fukushima@nnct
 * @brief crc生成/検証ライブラリ
 * @version 0.1
 * @date 2022-12-20
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once
#include <Arduino.h>
/**
 * @brief CRC16(CCITT)を計算する
 * 
 * @param buf 配列ポインタ
 * @param len 配列長さ
 * @return uint16_t CRC16(CCITT)
 */
namespace CRC{
uint16_t calculate(uint8_t *buf, size_t len)
{
    // crc16-ccitst
    uint16_t crc = 0xFFFF;
    for (int i = 0; i < len; i++)
    {
        crc ^= buf[i];
        for (int j = 0; j < 8; j++)
        {
            if (crc & 1)
            {
                crc >>= 1;
                crc ^= 0x8408;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return crc;
}
/**
 * @brief CRC16(CCITT)を検証する
 * 
 * @param buf 検証対象配列ポインタ
 * @param len 検証対象配列長さ
 * @param crc 検証対象CRC16(CCITT)
 * @return true 合致
 * @return false 合致しない
 */
inline bool check(uint8_t* buf,size_t len,uint16_t crc){
    return calculate(buf, len) == crc;
}
};