#ifdef FLAG_R6003
#include "can.h"
#include "board/R6003.h"
#include "wrapper.h"
#include <SPI.h>
namespace can
{
    ACAN2517 mcp2517fd(R6003::SPI_CS, SPI, 255);
    ACAN2517Filters filters;
    vSerial vserial;

    // call before init
    void addfilter(uint16_t id, ACANCallBackRoutine callback)
    {
        filters.appendFrameFilter(kStandard, id, callback);
    }

    bool led(uint8_t sel, bool on)
    {
        mcp2517fd.gpioWrite(sel, on);
        return false;
    }
    void publish(packet_t &packet)
    {
        CANMessage message;
        message.id = packet.id;
        message.len = packet.length;
        memcpy(message.data, packet.data, packet.length);
        if(!mcp2517fd.tryToSend(message)){
            Serial.println("[CAN] send error");
            while (1);
            
        }
    }
    bool receive(packet_t &packet)
    {
        // isrで処理するためコメントアウト

        // CANMessage message;
        // if(mcp2517fd.receive(message)){
        //     packet.id = message.id;
        //     packet.length = message.len;
        //     memcpy(packet.data, message.data, message.len);
        //     return true;
        // }
        return false;
    }
    void init()
    {
        SPI.begin();
        ACAN2517Settings settings = ACAN2517Settings(ACAN2517Settings::OSC_40MHz, 1000UL * 1000UL);//ULをつけろ！！！こいつは裏切り者だ！！！
        settings.mRequestedMode = ACAN2517Settings::Normal20B; // InternalLoopBack to test
        settings.mDriverReceiveFIFOSize = 1;
        settings.mDriverTransmitFIFOSize = 1;
        uint32_t err = mcp2517fd.begin(settings, nullptr, filters);

        if(!err){
            Serial.println("[CAN] init success");
            Serial.printf("[CAN] actual baud:%lu",settings.actualBitRate());
        }
        else{
            Serial.print("[CAN] init error:");
            Serial.println(err);
        }
        mcp2517fd.gpioSetMode(0, OUTPUT);
        mcp2517fd.gpioSetMode(1, OUTPUT);
    }
    int ot = 0;
    void update()
    {
        mcp2517fd.poll();
        mcp2517fd.dispatchReceivedMessage();
        if(millis()/100 - ot > 10){
            ot = millis()/100;
            int ec = mcp2517fd.errorCounters();
            // Serial.printf("[can]errcount:%d,currentopmode:%d\n",ec,mcp2517fd.currentOperationMode());
            if(ec > 0){
                init();
                Serial.println(mcp2517fd.recoverFromRestrictedOperationMode());
            }
        }
    }


};
#endif