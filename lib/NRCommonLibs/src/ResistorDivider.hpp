/**
 * @file ResistorDivider.h
 * @author K.Fukushima@nnct ( )
 * @brief 抵抗分圧の計算を行うクラス。計算だけ。
 * @version 0.1
 * @date 2024-07-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */
//resistance1は入力電圧側、resistance2はGND側の抵抗値
#pragma once
#include <stdint.h>
class ResistorDivider {
    public:
    ResistorDivider(float resistance1, float resistance2,float adc_ref=3.3,uint16_t adc_bits=12) {
        multiplier = (resistance1 + resistance2) / resistance2 * adc_ref / (((uint32_t)1<<adc_bits)-1);
    }
    float getVoltage(uint16_t adc_value) {
        return adc_value * multiplier;
    }
    private:
    float multiplier;
};