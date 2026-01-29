/**
 * @file Controller.hpp
 * @author K.Fukushima@nnct (21327@g.nagano-nct.ac.jp)
 * @brief コントローラーデータ管理ライブラリ
 * @version 0.4
 * @date 2024-07-22
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#include <stdint.h>
#include <string.h>
#include <math.h>

#define USE_PACKETIZER
// packetizer使わん時はNO_PACKETIZERを定義してください
#ifdef NO_PACKETIZER
#undef USE_PACKETIZER
#endif

#ifdef USE_PACKETIZER
#include "Packetizer.hpp"
#endif

#ifndef PI
#define PI 3.14159265358979323846
#endif

#ifndef constrain
#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#endif
// arduino map
#define ardmap(x, in_min, in_max, out_min, out_max) (constrain((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min, out_min, out_max))

/*
使い方
ControllerDataはコントローラーのボタン、アナログ値(スティック、トリガー)を管理するクラスで。
昔ながらの製法に現代風の味付けをしたもの(福島入学以前からの形式と互換性あり。送信機との互換性のためいまだに5バイト)
送信機側ではsetButton,setAnalogで値を設定。受信機側ではbutton,analograwで値を取得する。

ControllerManagerは入手したControllerDataを活用しやすくするためのクラスで、updateでControllerDataを渡すと、それをもとに
ボタンの押下された瞬間、離された瞬間、押され続けているか、変化したかの判定
アナログ値の整形、取得
異常値のつまみ出し
などを行うことができる。

ボタン、スティックの座標などにはIDが割り当てられ、controller::Indexでアクセスする。

使い方の例
ControllerManager manager;
ControllerData data;
...
[受信処理]
{
manager.update(data);
[データ活用]
if(manager.isPressed(controller::A))
{
//Aが押された時
}
int speed=10*manager.getValue(controller::LstickX);//-14~14:左スティックの左～右に倒した状態
//speedは-140~140に対応
}

 */

namespace controller
{
    // ボタンのID
    enum Index : uint8_t
    {
        // 英字ボタン
        A,
        B,
        X,
        Y,
        // 十字ボタン
        UP,
        DOWN,
        LEFT,
        RIGHT,
        // その他
        L,
        R,
        BACK,             // XBOXの左側
        START,            // XBOXの右側
        SL,               // 左スティック押し込み
        SR,               // 右スティック押し込み
        XBOX,             // XBOXボタン
        FLAG_STICK_POLAR, // 極座標/直交座標切り替え 0:直交座標 1:極座標

        // アナログ値のID
        LstickX = 64,
        LstickY,
        RstickX,
        RstickY,
        TriggerR,
        TriggerL,
    };
    // using namespace NRcomm;
    class ControllerData
    {
    public:
        ControllerData()
        {
            Button = 0;
            memset(Analogue, 0, sizeof(Analogue));
        }
        bool operator[](controller::Index i)
        {
            return button(i);
        }
#ifdef USE_PACKETIZER
        packet_t packetize(Packetizer &p)
        {
            packet_t packet;
            p.init(packet);
            p
                .pack(Button)
                .pack(Analogue, 3);
            return packet;
        }
        packet_t packetize()
        {
            Packetizer p;
            return packetize(p);
        }
        bool unpacketize(Packetizer &p)
        {
            return p
                .unpack(Button)
                .unpack(Analogue, 3)
                .success();
        }
        bool unpacketize(packet_t &packet)
        {
            Packetizer p;
            p.init(packet);
            return unpacketize(p);
        }
#endif
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
            if (a > 15)
                a = 15;
            if (i > TriggerL || i < LstickX)
            {
                return;
            }
            if (i % 2 != 0)
            {
                Analogue[(i - 64) / 2] &= 0xF0;
                Analogue[(i - 64) / 2] |= a & 0x0F;
            }
            else
            {
                Analogue[(i - 64) / 2] &= 0x0F;
                Analogue[(i - 64) / 2] |= (a & 0x0F) << 4;
            }
        }
        uint8_t getAnalog(controller::Index i)
        {
            if (i > TriggerL || i < LstickX)
            {
                return 0;
            }
            return (i % 2 != 0) ? Analogue[(i - 64) / 2] & 0x0F : Analogue[(i - 64) / 2] >> 4;
        }

        // deprecate
        inline uint8_t analograw(controller::Index i) { return getAnalog(i); };

        uint16_t Button;
        // uint8_t Lstick;  // Y,X
    private:
        // uint8_t Rstick;  // Y,X
        // uint8_t Trigger; // L,R
        uint8_t Analogue[3];
    };
    class ControllerManager
    {
    public:
        // 簡易複素数クラス　直交座標の利用時に使用

        ControllerManager() {}

        /**
         * @brief 整理した値を取得
         *
         * @param i
         * @return uint8_t スティックは-14~14,トリガーは0~15,ボタンは0,1
         */
        int16_t getValue(Index i)
        {
            switch (i)
            {
            case LstickX:
                return stick.L.real; //-16~16
            case LstickY:
                return stick.L.imag;
            case RstickX:
                return stick.R.real;
            case RstickY:
                return stick.R.imag;

            default:
                return getRaw(i);
            }
        }

        /**
         * @brief Get the Stick Polar Left object
         *
         * @param d
         * @param theta
         */
        void getStickPolarLeft(uint8_t &r, float &theta)
        {
            r = stick.L.abs();
            theta = stick.L.arg();
        }
        /**
         * @brief Get the Stick Polar Right object
         *
         * @param d
         * @param theta
         */
        void getStickPolarRight(uint8_t &r, float &theta)
        {
            r = stick.R.abs();
            theta = stick.R.arg();
        }

        /**
         * @brief 生の値を取得
         *
         * @param i
         * @return uint8_t スティックは0~15(送信時ママ),トリガーは0~15,ボタンは0,1
         */
        uint8_t getRaw(Index i)
        {
            if (i <= 15)
                return ctrl.button(i);
            return ctrl.analograw(i);
        }
        ControllerData getControllerData()
        {
            return ctrl;
        }
        /**
         * @brief 押下された瞬間のみtrue
         *
         * @param b
         * @return true
         * @return false
         */
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
        /**
         * @brief 離された瞬間のみtrue
         *
         * @param b
         * @return true
         * @return false
         */
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
        /**
         * @brief 押され続けてるときtrue
         *
         * @param b
         * @return true
         * @return false
         */
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

        /**
         * @brief 変化した瞬間のみtrue
         *
         * @param i
         * @return true
         * @return false
         */
        bool isChanged(Index i)
        {
            if (i > 15)
                return ctrl.analograw(i) != ctrl_old.analograw(i);
            return ctrl[i] != ctrl_old[i];
        }

        // データの管理
        /**
         * @brief データのリセット（初期化）
         *
         */
        void clear()
        {
            memset(&ctrl, 0, sizeof(ControllerData));
            memset(&ctrl_old, 0, sizeof(ControllerData));
            update(ctrl);
        }

        /**
         * @brief データの更新
         *
         * @param c
         */
        void update(ControllerData &c)
        {
            ctrl_old = ctrl;
            ctrl = c;
            stick_old = stick;
            update_difference();
        }

    private:
        struct lightcomplex
        {
            lightcomplex(int8_t r, int8_t i) : real(r), imag(i) {}
            int8_t real;
            int8_t imag;
            int8_t abs()
            {
                return (int8_t)sqrt((int16_t)real * (int16_t)real + (int16_t)imag * (int16_t)imag);
            }
            float arg()
            {
                return (float)atan2(imag, real);
            }
            void polar(uint8_t d, uint8_t theta16)
            {
                real = (int8_t)((float)d * (float)cos((float)theta16 * PI / 8));
                imag = (int8_t)((float)d * (float)sin((float)theta16 * PI / 8));
            };
        };
        ControllerData ctrl;
        ControllerData ctrl_old{};
        uint8_t threshold = 8; // トリガー押し込みをボタンとして扱うときのしきい値
        struct
        {
            lightcomplex L{0, 0}; //-15~15
            lightcomplex R{0, 0}; //-15~15
        } stick, stick_old;
        /**
         * @brief 差分を反映
         *
         */
        void update_difference()
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
                    stick.L.real = ardmap(ctrl.analograw(LstickX) - 8, -8, 7, -15, 15);
                    stick.L.imag = ardmap(ctrl.analograw(LstickY) - 8, -8, 7, -15, 15);
                }
                if (ctrl.analograw(RstickX) == 0 && ctrl.analograw(RstickY) == 0)
                {
                    stick.R.polar(0, 0);
                }
                else
                {
                    stick.R.real = ardmap(ctrl.analograw(RstickX) - 8, -8, 7, -15, 15);
                    stick.R.imag = ardmap(ctrl.analograw(RstickY) - 8, -8, 7, -15, 15);
                }
                // Serial.printf("%f %f\n", stick.L.real(), stick.L.imag());
            }
        }
    };
};