#pragma once
#include "Packetizer.hpp"

namespace datastruct
{
    // R6005
    enum R6005DataID
    {
        ID_STATUS = 0,
        ID_MU = 1,
    };
    class R6005Status
    {
    public:
        enum StatusIdx
        {
            STATUS_EMG = 0,        // 非常停止状態
            STATUS_CIRC = 1,       // 回路電源オンオフ
            STATUS_MECH = 2,       // 機構電源オンオフ
            STATUS_CTRLONLINE = 3, // コントローラ接続状態
            STATUS_EMG_SW = 4,     // EMGスイッチ状態 true=非常停止
            STATUS_EMG_MU = 5,     // MU非常停止状態 true=非常停止
            STATUS_OUT_FORCE = 6,  // 出力強制オンオフ（物理以外の非常停止無視）
        };
        packet_t packetize(Packetizer &p)
        {
            packet_t packet;
            p.init(packet);
            p
                .pack(status)
                .pack(voltage_mech)
                .pack(current_mech);
            return packet;
        }
        bool unpacketize(Packetizer &p)
        {
            return p
                .unpack(status)
                .unpack(voltage_mech)
                .unpack(current_mech)
                .success();
        }
        void setStatus(StatusIdx idx, bool value)
        {
            if (value)
            {
                status |= 1 << idx;
            }
            else
            {
                status &= ~(1 << idx);
            }
        }
        bool getStatus(StatusIdx idx)
        {
            return status & (1 << idx);
        }
        void setMechVoltage(uint8_t value)
        {
            voltage_mech = value;
        }
        uint8_t getVoltageMech()
        {
            return voltage_mech;
        }
        void setMechCurrent(uint8_t value)
        {
            current_mech = value;
        }
        uint8_t getCurrentMech()
        {
            return current_mech;
        }

    private:
        uint16_t status;
        uint8_t voltage_mech; //*10V
        uint8_t current_mech; // *5A
    };
    class KarastrofaStatus
    {
    public:
        enum StatusIdx
        {
            STATUS_CTRLONLINE = 1, // コントローラ接続状態
            STATUS_EMG_MU = 2,     // MU非常停止状態 true=非常停止
        };
        packet_t packetize(Packetizer &p)
        {
            packet_t packet;
            p.init(packet);
            p
                .pack(status)
                .pack(voltage_mech)
                .pack(current_mech);
            return packet;
        }
        bool unpacketize(Packetizer &p)
        {
            return p
                .unpack(status)
                .unpack(voltage_mech)
                .unpack(current_mech)
                .success();
        }
        void setStatus(StatusIdx idx, bool value)
        {
            if (value)
            {
                status |= 1 << idx;
            }
            else
            {
                status &= ~(1 << idx);
            }
        }
        bool getStatus(StatusIdx idx)
        {
            return status & (1 << idx);
        }
        void setMechVoltage(uint8_t value)
        {
            voltage_mech = value;
        }
        uint8_t getVoltageMech()
        {
            return voltage_mech;
        }
        void setMechCurrent(uint8_t value)
        {
            current_mech = value;
        }
        uint8_t getCurrentMech()
        {
            return current_mech;
        }

    private:
        uint16_t status;
        uint8_t voltage_mech; //*10V
        uint8_t current_mech; // *5A
    };
    enum R6003DataID
    {
        ID_Sensor = 0,
        ID_VSerial = 1,
    };
    class R6003Status
    {
    public:
        enum StatusIdx
        {
            a,
            b
        };
        packet_t packetize(Packetizer &p)
        {
            packet_t packet;
            p.init(packet);
            p
                .pack(status)
                .pack(voltage_mech)
                .pack(current_mech);
            return packet;
        }
        bool unpacketize(Packetizer &p)
        {
            return p
                .unpack(status)
                .unpack(voltage_mech)
                .unpack(current_mech)
                .success();
        }
        void setStatus(StatusIdx idx, bool value)
        {
            if (value)
            {
                status |= 1 << idx;
            }
            else
            {
                status &= ~(1 << idx);
            }
        }
        bool getStatus(StatusIdx idx)
        {
            return status & (1 << idx);
        }
        void setMechVoltage(uint8_t value)
        {
            voltage_mech = value;
        }
        uint8_t getVoltageMech()
        {
            return voltage_mech;
        }
        void setMechCurrent(uint8_t value)
        {
            current_mech = value;
        }
        uint8_t getCurrentMech()
        {
            return current_mech;
        }

    private:
        uint16_t status;
        uint8_t voltage_mech; //*10V
        uint8_t current_mech; // *5A
    };

};
