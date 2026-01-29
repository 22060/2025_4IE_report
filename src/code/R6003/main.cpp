// #include <Arduino.h>
#ifdef FLAG_R6003
// #include "controller.h"
// #include "id.h"
#include "board/R6003.h"
#include <Arduino.h>
#include "can.h"
#include "Packetizer.hpp"
// #include "id.h"
#include "tinytasker.h"
#include "datastruct.h"
Packetizer packetizer;
using namespace datastruct;
R6003Status status;
R6005Status data6005;

// controller::ControllerManager c;
void ctrl_update(const CANMessage &msg)
{
    // packet_t p;
    // packetizer.init(p).set(msg.data, 8);
    // c.ctrl.unpacketize(packetizer);
    // c.update();
}
void mainstatus_update(const CANMessage &msg)
{
    packet_t p;
    packetizer.init(p).set(msg.data, 8);
    data6005.unpacketize(packetizer);
    can::led(0, data6005.getStatus(R6005Status::STATUS_EMG_SW));
}
void tryrobot(const CANMessage &msg)
{
    static int ltika = 0;
    analogWrite(R6003::M1_PWM, msg.data[4]);
    analogWrite(R6003::M2_PWM, msg.data[6]);
    digitalWrite(R6003::M1_INA, msg.data[5] % 2);
    digitalWrite(R6003::M1_INB, (msg.data[5] / 2) % 2);
    digitalWrite(R6003::M2_INA, msg.data[7] % 2);
    digitalWrite(R6003::M2_INB, (msg.data[7] / 2) % 2);
    Serial.printf("s%c%c%c%c%c%c%c%c\n", msg.data[0],msg.data[1],msg.data[2],msg.data[3],msg.data[4],msg.data[5],msg.data[6],msg.data[7]);
    // Serial.printf("c %d\n", msg.data[0]);
    can::led(1, ltika);
    ltika = !ltika;
    // can::vserial.printf("%d %d\n",msg.data[0],msg.data[1]);
}
void pass(const CANMessage &msg)
{
    static int ltika = 0;
    // analogWrite(R6003::M2_PWM, msg.data[0]);
    // analogWrite(R6003::M1_PWM, msg.data[1]);
    // digitalWrite(R6003::M1_INA, msg.data[2]);
    // digitalWrite(R6003::M1_INB, msg.data[3]);
    // digitalWrite(R6003::M2_INA, msg.data[4]);
    // digitalWrite(R6003::M2_INB, msg.data[5]);
    Serial.printf("s %d %d %d %d\n", msg.data[0],msg.data[1],msg.data[2],msg.data[3]);//servo1 servo2 servo3 mode
    can::led(1, ltika);
    ltika = !ltika;
    // can::vserial.printf("%d %d\n",msg.data[0],msg.data[1]);
}

void peripheral_init()
{
    // can::addfilter(id::R6005a | id::R6005::ID_MU, ctrl_update);
    // can::addfilter(id::R6005a | id::R6005::ID_STATUS0, mainstatus_update);
    can::addfilter(0x123, tryrobot);
    can::addfilter(0x124, tryrobot);
    can::init();
    can::led(0, true);
    can::led(1, true);
    delay(100);
    can::led(0, false);
    can::led(1, false);
}
void tPubSensor()
{
    // mySerial.println("loop");
}
tinytasker tkPubSensor(100, tPubSensor);
tinytasker tLTika(500, []()
                  {
                      static bool ltika = false;
                      can::led(0, ltika);
                      ltika = !ltika;
                      // Serial.print("ltika:");
                      // Serial.println(ltika);
                  });
void tPubTest()
{
    // can::vserial.print("あああああtestprintaaaaabbbbbcccccdddddeeeeefffffggggghhhhhiiiiijjjfjdkalsjfeipoajfoipdjaoipvjeoijafoijciojoiajwopjaeiovpjapfiopenawofnopoieanvovneoanfoiewnopfnoiewnfeawnajj:");
    // can::vserial.println(millis());
}
// tinytasker tkPubTest(10, tPubTest);
void setup()
{
    // M1INA(PC0,TCD0)
    TCA0_SPLIT_CTRLA = (TCA_SPLIT_CLKSEL_DIV8_gc | TCA_SPLIT_ENABLE_bm);
    // 上記と同じ分周、周波数のため同様に周波数変更可能
    // PWM代替ピン設定、騙す
    // set
    delay(2000);
    Serial.begin(115200);
    delay(500);
    peripheral_init();
    pinMode(R6003::M1_PWM, OUTPUT);
    pinMode(R6003::M1_INA, OUTPUT);
    pinMode(R6003::M1_INB, OUTPUT);
    pinMode(R6003::M2_INA, OUTPUT);
    pinMode(R6003::M2_INB, OUTPUT);
    pinMode(R6003::M2_PWM, OUTPUT);
    digitalWrite(R6003::M1_INB, HIGH);
    digitalWrite(R6003::M1_INA, LOW);
    digitalWrite(R6003::M1_PWM, LOW);
    digitalWrite(R6003::M2_INB, HIGH);
    digitalWrite(R6003::M2_INA, LOW);
    digitalWrite(R6003::M2_PWM, LOW);
    // Serial.println(1000 * 1000); //??????
    // PORTMUX_CTRLC |= PORTMUX_TCA02_bm;
}
void sendsensordata()
{
    packet_t p;
    p.id = 0x123;
    p.length = 8;
    p.data[0] = 10;
    p.data[1] = 11;
    p.data[2] = 12;
    p.data[3] = 13;
    p.data[4] = 14;
    p.data[5] = 15;
    p.data[6] = 16;
    p.data[7] = 17;

    can::publish(p);
    // Serial.println("loop");
}
tinytasker tSendSensor(10, sendsensordata);
// analogwrite in 0,255 does not work
int num = 0;
int ot = 0;
void loop()
{
    // status.setMechVoltage(millis() % 100);
    can::update();
    // tkPubSensor.update();
    // tLTika.update();
    // tkPubTest.update();
    // tSendSensor.update();
    // analogWrite(R6003::M2_PWM, num);
    // if (Serial.available())
    // {
    //     char[3] buf = Serial.readStringUntil('\n');
    //     int val = buf[1];
    //     uint8_t pwm = buf[2];
    //     // Serial.print("Received from Serial1: ");
    // if(val % 2 == 1){
    //     digitalWrite(R6003::M1_INA,1);
    //     digitalWrite(R6003::M1_INB,0);
    //     digitalWrite(R6003::M2_INA,1);
    //     digitalWrite(R6003::M2_INB,0);
    // }else{
    //     digitalWrite(R6003::M1_INA,0);
    //     digitalWrite(R6003::M1_INB,1);
    //     digitalWrite(R6003::M2_INA,0);
    //     digitalWrite(R6003::M2_INB,1);
    // }
    // if((val / 2) % 2 == 1){
    //     analogWrite(R6003::M2_PWM, pwm);
    //     analogWrite(R6003::M1_PWM, pwm);
    // }else{
    //     analogWrite(R6003::M2_PWM, 0);
    //     analogWrite(R6003::M1_PWM, 0);
    // }
    // }
}
#endif