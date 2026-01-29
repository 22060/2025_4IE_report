#pragma once
#include <stdint.h>
#include "Packetizer.hpp"
class NRCommPort
{
public:
virtual void send(packet_t &packet) = 0;
virtual bool receive(packet_t &packet) = 0;
virtual void addFilter(uint32_t id,uint32_t filter,void(*callback)(packet_t &packet)=NULL) = 0;

};