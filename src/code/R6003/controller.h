/**
 * @file controller.h
 * @author K.Fukushima@nnct (21327@g.nagano-nct.ac.jp)
 * @brief コントローラーデータ管理ライブラリ
 * @version 0.2
 * @date 2024-04-04
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include <stdint.h>
#include "Packetizer.hpp"
#include <math.h>
#ifndef PI
#define PI 3.14159265358979323846
#endif
namespace controller
{
    enum Index : uint8_t
    {
        A,
        B,
        X,
        Y,
        UP,
        DOWN,
        LEFT,
        RIGHT,
        L,
        R,
        BACK,
        START,
        SL,
        SR,
        XBOX,
        FLAG_STICK_POLAR,
        LstickX = 64,
        LstickY,
        RstickX,
        RstickY,
        TriggerL,
        TriggerR,
    };
    using namespace NRcomm;
    class ControllerData
    {
    public:
        bool operator[](controller::Index i)
        {
            return button(i);
        }
        packet_t packetize(Packetizer &p)
        {
            packet_t packet;
            p.init(packet);
            p
                .pack(Button)
                .pack(Analogue, 3);
            return packet;
        }
        bool unpacketize(Packetizer &p)
        {
            return p
                .unpack(Button)
                .unpack(Analogue, 3)
                .success();
        }
        inline bool button(controller::Index i)
        {
            if (i > 15)
                return false;
            return (Button & (1 << i));
        }
        void setButton(controller::Index i, bool b)
        {
            if (i > 15)
                return;
            if (b)
                Button |= (1 << i);
            else
                Button &= ~(1 << i);
        }
        void setAnalog(controller::Index i, uint8_t a)
        {
            if(a>15)a=15;
            if (i >= TriggerR || i < LstickX)
            {
                if (i % 2 == 0)
                {
                    Analogue[(64 - i) / 2] &= 0xF0;
                    Analogue[(64 - i) / 2] |= a & 0x0F;
                }
                else
                {
                    Analogue[(64 - i) / 2] &= 0x0F;
                    Analogue[(64 - i) / 2] |= (a & 0x0F) << 4;
                }
            }
        }
        
        inline uint8_t analograw(controller::Index i)
        {
            if (i >= TriggerR || i < LstickX)
            {
                return (i % 2 == 0) ? Analogue[(64 - i) / 2] & 0x0F : Analogue[(64 - i) / 2] >> 4;
            }
            return 0;
        }

    private:
        uint16_t Button;
        // uint8_t Lstick;  // Y,X
        // uint8_t Rstick;  // Y,X
        // uint8_t Trigger; // L,R
        uint8_t Analogue[3];
    };
    class ControllerManager
    {
    public:
        struct lightcomplex
        {
            lightcomplex(int8_t r, int8_t i) : real(r), imag(i) {}
            int8_t real;
            int8_t imag;
            int8_t abs()
            {
                return sqrt((int16_t)real * (int16_t)real + (int16_t)imag * (int16_t)imag);
            }
            int8_t arg()
            {
                return atan2(imag, real);
            }
            void polar(int8_t d, int8_t theta16)
            {
                real = d * cos((float)theta16 * PI / 8);
                imag = d * sin((float)theta16 * PI / 8);
            };
        };
        ControllerManager() {}
        ControllerData ctrl;
        ControllerData ctrl_old{};
        uint8_t threshold = 8;
        struct
        {
            lightcomplex L{0, 0}; //-15~15
            lightcomplex R{0, 0}; //-15~15
        } stick, stick_old;
        uint8_t raw(Index i)
        {
            if (i <= 15)
                return ctrl.button(i);
            return ctrl.analograw(i);
        }
        bool isPressed(Index b)
        {
            if (b > 15)
            {
                if (b == TriggerL)
                    return ctrl.analograw(TriggerL) > threshold && ctrl_old.analograw(TriggerL) <= threshold;
                if (b == TriggerR)
                    return ctrl.analograw(TriggerR) > threshold && ctrl_old.analograw(TriggerR) <= threshold;
                return false;
            }
            return ctrl.button(b) && !ctrl_old.button(b);
        }
        bool isReleased(Index b)
        {
            if (b > 15)
            {
                if (b == TriggerL)
                    return ctrl.analograw(TriggerL) <= threshold && ctrl_old.analograw(TriggerL) > threshold;
                if (b == TriggerR)
                    return ctrl.analograw(TriggerR) <= threshold && ctrl_old.analograw(TriggerR) > threshold;
                return false;
            }
            return !ctrl.button(b) && ctrl_old.button(b);
        }
        bool isHold(Index b)
        {
            if (b > 15)
            {
                if (b == TriggerL)
                    return ctrl.analograw(TriggerL) > threshold;
                if (b == TriggerR)
                    return ctrl.analograw(TriggerR) > threshold;
                return false;
            }
            return ctrl.button(b);
        }
        bool changed(Index i)
        {
            if (i > 15)
                return ctrl.analograw(i) != ctrl_old.analograw(i);
            return ctrl[i] != ctrl_old[i];
        }
        void clear()
        {
            memset(&ctrl, 0, sizeof(ControllerData));
            memset(&ctrl_old, 0, sizeof(ControllerData));
            update();
        }
        void updateChange()
        {
            ctrl_old = ctrl;
            stick_old = stick;
        }
        void update()
        {
            if (ctrl.button(Index::FLAG_STICK_POLAR))
            {
                stick.L.polar(ctrl.analograw(Index::LstickX), ctrl.analograw(Index::LstickY));
                stick.R.polar(ctrl.analograw(Index::RstickX), ctrl.analograw(Index::RstickY));
            }
            else
            {
                if (ctrl.analograw(LstickX) == 0 && ctrl.analograw(LstickY) == 0)
                {
                    stick.L.polar(0, 0);
                }
                else
                {
                    stick.L.real =ctrl.analograw(LstickX) - 8;
                    stick.L.imag =ctrl.analograw(LstickY) - 8;
                }
                if (ctrl.analograw(RstickX) == 0 && ctrl.analograw(RstickY) == 0)
                {
                    stick.R.polar(0, 0);
                }
                else
                {
                    stick.R.real =ctrl.analograw(RstickX) - 8;
                    stick.R.imag =ctrl.analograw(RstickY) - 8;
                }

                // Serial.printf("%f %f\n", stick.L.real(), stick.L.imag());
            }
        }
    };
};