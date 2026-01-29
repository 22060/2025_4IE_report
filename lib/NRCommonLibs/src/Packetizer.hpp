/**
 * @file Packetizer.hpp
 * @author K.Fukushima@nnct (21327@g.nagano-nct.ac.jp)
 * @brief 型を問わずバイト列に詰めこむライブラリ　エンディアンを考慮
 * @version 0.2
 * @date 2024-04-09
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#include <stdint.h>
#include <string.h>
#if __has_include(<machine/_endian.h>)
#define LittleEndian (BYTE_ORDER == LITTLE_ENDIAN)
#include <sys/param.h>
#else
#define LittleEndian 1
#endif
// パケットを取り込んで、順次処理をおこなう。
template <uint8_t size>
struct Packet
{
    uint16_t id;
    uint8_t data[size];
    uint8_t length;
    Packet()
    {
        memset(data, 0, size);
        length = 0;
    }
};
using packet_t = Packet<8>; // deprecated
template <uint8_t size>
using PacketCallback = void (*)(Packet<size> &packet);
class Packetizer
{
public:
    enum error_t
    {
        ERR_BUFFER_OVERFLOW,
        ERR_INVALID_LENGTH
    };
    Packetizer &setErrCallback(void (*callback)(error_t err))
    {
        err_callback = callback;
        return *this;
    };
    Packetizer(){

    };
    ~Packetizer(){

    };
    template <uint8_t size=8>
    Packetizer &init(Packet<size> &packet, bool clear = false)
    {
        error = false;
        packet_pbuf = packet.data;
        packet_plen = &packet.length;
        packet_size = size;
        if (clear)
        {
            memset(packet_pbuf, 0, packet_size);
            *packet_plen = 0;
        }
        // packet_->id = 0;
        unpack_index = 0;
        return *this;
    };
    Packetizer &set(const uint8_t *data, uint8_t length)
    {
        if (error)
            return *this;
        memcpy(packet_pbuf, data, length);
        *packet_plen = length;
        return *this;
    };

    template <typename T>
    Packetizer &pack(T data)
    {
        if (error)
            return *this;
#if LittleEndian
        if (*packet_plen + sizeof(T) > packet_size)
        {
            err_callback(ERR_BUFFER_OVERFLOW);
            error = true;
            return *this;
        }
        memcpy(packet_pbuf + *packet_plen, &data, sizeof(T));
#else
        if (*packet_plen + sizeof(T) > buf_size)
        {
            err_callback(ERR_BUFFER_OVERFLOW);
            return *this;
        }
        for (int i = 0; i < sizeof(T); i++)
        {
            packet_pbuf[*packet_plen + i] = ((uint8_t *)&data)[sizeof(T) - i - 1];
        }
#endif
        *packet_plen += sizeof(T);
        return *this;
    };

    Packetizer &pack(uint8_t *data, uint8_t length)
    {
        if (error)
            return *this;
        if (*packet_plen + length > packet_size)
        {
            err_callback(ERR_BUFFER_OVERFLOW);
            error = true;
            return *this;
        }
        memcpy(packet_pbuf + *packet_plen, data, length);
        *packet_plen += length;
        return *this;
    };

    template <typename T>
    Packetizer &unpack(T &data)
    {
        if (error)
            return *this;
        if (unpack_index + sizeof(T) > packet_size)
        {
            err_callback(ERR_INVALID_LENGTH);
            return *this;
        }
#if LittleEndian
        memcpy(&data, packet_pbuf + unpack_index, sizeof(T));
        unpack_index += sizeof(T);
#else
        for (int i = 0; i < sizeof(T); i++)
        {
            ((uint8_t *)&data)[i] = packet_pbuf[unpack_index + sizeof(T) - i - 1];
        }
        unpack_index += sizeof(T);
#endif
        return *this;
    };
    Packetizer &unpack(uint8_t *data, uint8_t length)
    {
        if (error)
            return *this;
        if (unpack_index + length > packet_size)
        {
            err_callback(ERR_INVALID_LENGTH);
            return *this;
        }
        memcpy(data, packet_pbuf + unpack_index, length);
        unpack_index += length;
        return *this;
    };
    bool success()
    {
        return !error;
    }

private:
    // packet_t* packet_=nullptr;
    uint8_t *packet_pbuf = nullptr;
    uint8_t *packet_plen = nullptr;
    uint8_t unpack_index;
    void (*err_callback)(error_t err);
    bool error = true;
    uint8_t packet_size = 0;
};
