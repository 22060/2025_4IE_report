#pragma once
#include <wrapper.h>
#include <ACAN2517.h>
#include <Arduino.h>
#include "id.h"
#include "Packetizer.hpp"
#include "datastruct.h"

namespace can {
    void addfilter(uint16_t id, ACANCallBackRoutine callback);
    void init();
    void publish(packet_t &packet);
    bool receive(packet_t &packet);
    void update();
    bool led(uint8_t sel, bool on);
    class vSerial : public Print
    {
    public:
        vSerial(){}
        // virtual int available(){return 0;};
        // virtual int read(){return 0;};
        // virtual int peek(){return 0;};
        virtual void flush(){
            sendBuffer();
        }
        virtual size_t write(uint8_t c){
            packet.data[packet.length]=c;
            packet.length++;
            if(packet.length>=8){
                sendBuffer();
            }
        }
        virtual size_t write(const uint8_t *buffer, size_t size){
            while(size>=8){
                memcpy(packet.data,buffer,8);
                buffer+=8;
                packet.length=8;
                sendBuffer();
                size-=8;
            }
            memcpy(packet.data,buffer,size);
            packet.length=size;
            sendBuffer();
        }
    private:
        packet_t packet;
        void sendBuffer(){
            packet.id=id::R6003a | datastruct::R6003DataID::ID_VSerial;
            publish(packet);
            packet.length=0;

        }
    
    };
    extern vSerial vserial;
};
