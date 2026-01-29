#pragma once
#include <stdint.h>
#include "Packetizer.hpp"
#include "wrapper.h"
namespace NRcomm
{
    typedef  uint16_t id_t;
    class Manager
    {
    public:
        struct sub_t{
            id_t id;
            void (*callback)(packet_t &packet);
        };
        enum error_t
        {
            ERR_BUFFER_OVERFLOW,
            ERR_INVALID_LENGTH,
            ERR_SUBSIZE_OVERFLOW,
        };
        Manager(NRCommPort &port, uint8_t sub_max = 8):port(&port),subSize(sub_max){
            sub = new sub_t[subSize];
            subIndex = 0;
        };
        ~Manager(){
            delete[] sub;
        };

        bool setErrCallback(void (*callback)(error_t err))
        {
            errcallback = callback;
        }
        bool subscribe(id_t id,void(*callback)(packet_t&),id_t mask)
        {

            if(subIndex >= subSize){
                if(errcallback) errcallback(ERR_SUBSIZE_OVERFLOW);
                return false;
            }
            sub[subIndex].id = id;
            sub[subIndex].callback = callback;
            subIndex++;

            return true;
        }

        bool publish(id_t id, uint8_t *data, uint8_t length){
            if(length > 8){
                if(errcallback) errcallback(ERR_INVALID_LENGTH);
                return false;
            }
            packet_t packet;
            packet.id = id;
            packet.length = length;
            memcpy(packet.data, data, length);
            port->send(packet);
        
        };
        bool update(){
            packet_t packet;
            if(port->receive(packet)){
                for(uint8_t i = 0; i < subSize; i++){
                    if(sub[i].id == packet.id){
                        if(sub[i].callback != NULL)
                            sub[i].callback(packet);
                    }
                }
                return true;
            }
            return false;
        };
    private:
        void (*errcallback)(error_t err);
        sub_t *sub;
        uint8_t subIndex=0;
        uint8_t subSize;
        NRCommPort *port;
    };

} // namespace NRcomm