#ifndef FE0B0E70_6301_4F1B_A7E8_171617C7F062
#define FE0B0E70_6301_4F1B_A7E8_171617C7F062
#include "Packetizer.hpp"
#include <stdint.h>
#ifdef FLAG_R6005
#include <Arduino.h>
#endif
/**
 * @brief このボードはメッセージID 0x
 *
 */
namespace R6003
{
    /**
     * @brief 通信
     *
     */
    // 上位5bitはボードID,下位5bitはメッセージID
    enum Pins : unsigned char
    {
        // M1_INA = 10,//PC0,TCB0_alt/TCD_3
        // M1_PWM = 12,//PC2,no pwm!!
        M1_INA=10,//PC0,TCD0
        M1_INB = 12,//PC2,nopwm
        M1_PWM = 9,//PB0,TCA0_0
        M1_CS = 11,//PC1
        M2_INA = 4,//PB5,TCA0_2_alt
        M2_INB = 0,//PA4,TCA0_4
        M2_CS = 5,//PB4
        M2_PWM = 8,//PB1,TCA0_1
        EXT1 = 1,//PA5
        EXT2 = 2,//PA6
        EXT3 = 3,//PA7
        TX = 7,//PB2
        RX = 6,//PB3
        SPI_MOSI = 14,//PA1
        SPI_MISO = 15,//PA2
        SPI_CS = 13,//PC3,alt position
        SPI_SCK = 16,//PA3
    };

}; // namespace R6005


#endif /* FE0B0E70_6301_4F1B_A7E8_171617C7F062 */
