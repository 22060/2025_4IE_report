#ifdef CMOSCAMERA
/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <Wire.h>
#include <MadgwickAHRS.h>
#define MPU6050_ADDR 0x68       // MPU-6050 device address
#define MPU6050_SMPLRT_DIV 0x19 // MPU-6050 register address
#define MPU6050_CONFIG 0x1a
#define MPU6050_GYRO_CONFIG 0x1b
#define MPU6050_ACCEL_CONFIG 0x1c
#define MPU6050_WHO_AM_I 0x75
#define MPU6050_PWR_MGMT_1 0x6b
Madgwick filter;
#define SCALE 0.5
#define CUCL_GYRO (65535.0 / SCALE / 2000.0)
#define CUCL_ACC (65535.0 / SCALE / 16.0)
#define V3_OUT 16
void calcRotation();
double offsetX = 0, offsetY = 0, offsetZ = 0;
double offsetaX = 0, offsetaY = 0, offsetaZ = 0;
float acc_x, acc_y, acc_z;
float xx, xy, xz, dpsX, dpsY, dpsZ, gx, gy, gz;
int16_t raw_acc_x, raw_acc_y, raw_acc_z, raw_t, raw_gyro_x, raw_gyro_y, raw_gyro_z;

void culcRotation();
// I2c書き込み
void writeMPU6050(byte reg, byte data)
{
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(reg);
    Wire.write(data);
    Wire.endTransmission();
}

// i2C読み込み
byte readMPU6050(byte reg)
{
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(reg);
    Wire.endTransmission(true);
    Wire.requestFrom(MPU6050_ADDR, 1 /*length*/);
    byte data = Wire.read();
    return data;
}
int timer_50ms = 0;
int microsd = 0;
void setup()
{
    filter.begin(200); // 200Hzのサンプリング周波数
    pinMode(V3_OUT, OUTPUT);
    // digitalWrite(V3_OUT, LOW);
    // delay(100);
    digitalWrite(V3_OUT, HIGH);
    delay(500);
    Wire.begin();
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(0x6B); // PWR_MGMT_1 register
    Wire.write(0);    // set to zero (wakes up the MPU-6050)
    Wire.endTransmission(true);

    Serial.begin(9600);
    delay(100);

    // 正常に接続されているかの確認
    if (readMPU6050(MPU6050_WHO_AM_I) != 0x68)
    {
        Serial.println("\nWHO_AM_I error.");
        while (true)
            ;
    }

    // 設定を書き込む
    writeMPU6050(MPU6050_SMPLRT_DIV, 0x00);  // sample rate: 8kHz/(7+1) = 1kHz
    writeMPU6050(MPU6050_CONFIG, 0x00);      // disable DLPF, gyro output rate = 8kHz
    writeMPU6050(MPU6050_GYRO_CONFIG, 0x18); // gyro range: ±2000dps
    writeMPU6050(MPU6050_ACCEL_CONFIG, 24);  // accel range: ±2g
    writeMPU6050(MPU6050_PWR_MGMT_1, 0x01);  // disable sleep mode, PLL with X gyro

    // キャリブレーション
    Serial.print("Calculate Calibration");
    for (int i = 0; i < 3000; i++)
    {

        int16_t raw_acc_x, raw_acc_y, raw_acc_z, raw_t, raw_gyro_x, raw_gyro_y, raw_gyro_z;

        Wire.beginTransmission(MPU6050_ADDR);
        Wire.write(0x3B);
        Wire.endTransmission(false);
        Wire.requestFrom(MPU6050_ADDR, 14, true);

        raw_acc_x = Wire.read() << 8 | Wire.read();
        raw_acc_y = Wire.read() << 8 | Wire.read();
        raw_acc_z = Wire.read() << 8 | Wire.read();
        raw_t = Wire.read() << 8 | Wire.read();
        raw_gyro_x = Wire.read() << 8 | Wire.read();
        raw_gyro_y = Wire.read() << 8 | Wire.read();
        raw_gyro_z = Wire.read() << 8 | Wire.read();
        dpsX = ((float)raw_gyro_x) / CUCL_GYRO;
        dpsY = ((float)raw_gyro_y) / CUCL_GYRO;
        dpsZ = ((float)raw_gyro_z) / CUCL_GYRO;
        acc_x = ((float)raw_acc_x) / CUCL_ACC;
        acc_y = ((float)raw_acc_y) / CUCL_ACC;
        acc_z = ((float)raw_acc_z) / CUCL_ACC;
        offsetX += dpsX;
        offsetY += dpsY;
        offsetZ += dpsZ;
        offsetaX += acc_x;
        offsetaY += acc_y;
        offsetaZ += acc_z;
        if (i % 1000 == 0)
        {
            Serial.print(".");
        }
    }
    Serial.println();

    offsetX /= 3000;
    offsetY /= 3000;
    offsetZ /= 3000;
    offsetaX /= 3000;
    offsetaY /= 3000;
    offsetaZ /= 3000;

    Serial.print("offsetX : ");
    Serial.print(offsetX);
    Serial.print(",offsetY : ");
    Serial.print(offsetY);
    Serial.print(",offsetZ : ");
    Serial.println(offsetZ);
    timer_50ms = millis();
    microsd = micros();
}
double vx = 0;
double vy = 0;
double ix = 0;
double iy = 0;
double lx = 0;
double ly = 0;
int vz = 0;
void loop()
{

    // レジスタアドレス0x3Bから、計14バイト分のデータを出力するようMPU6050へ指示
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU6050_ADDR, 14, true);

    // 出力されたデータを読み込み、ビットシフト演算
    raw_acc_x = Wire.read() << 8 | Wire.read();
    raw_acc_y = Wire.read() << 8 | Wire.read();
    raw_acc_z = Wire.read() << 8 | Wire.read();
    raw_t = Wire.read() << 8 | Wire.read();
    raw_gyro_x = Wire.read() << 8 | Wire.read();
    raw_gyro_y = Wire.read() << 8 | Wire.read();
    raw_gyro_z = Wire.read() << 8 | Wire.read();
    calcRotation();

    filter.updateIMU((dpsX - offsetX) / 2 * 3, (dpsY - offsetY) / 2 * 3, (1 * (dpsZ - offsetZ)) / 2 * 3, acc_x, acc_y, acc_z);
    vx += (double)(acc_x - offsetaX + lx) * (double)(micros() - microsd) / 10000;
    vy += (double)(acc_y - offsetaY + ly) * (double)(micros() - microsd) / 10000;
    lx = (acc_x - offsetaX);
    ly = (acc_y - offsetaY);
    xx += (vx * (double)(micros() - microsd) / 10000);
    xy += (vy * (double)(micros() - microsd) / 10000);
    microsd = micros();
    if (millis() - timer_50ms > 30)
    {
        timer_50ms = millis();
        gx = filter.getRoll();
        gy = filter.getPitch();
        gz = filter.getYaw();

        Serial.print(gx);
        Serial.print(",");
        Serial.print(gy);
        Serial.print(",");
        Serial.print(gz - 180);
        Serial.print(",");
        Serial.print(0);
        Serial.print(",");
        Serial.println(0 * 1000);
    }
}
// 加速度、ジャイロから角度を計算
void calcRotation()
{

    // 単位Gへ変換
    acc_x = ((float)raw_acc_x) / CUCL_ACC;
    acc_y = ((float)raw_acc_y) / CUCL_ACC;
    acc_z = ((float)raw_acc_z) / CUCL_ACC;
    // 単位rad/sへ変換
    dpsX = ((float)raw_gyro_x) / CUCL_GYRO; // LSB sensitivity: 65.5 LSB/dps @ ±2000dps
    dpsY = ((float)raw_gyro_y) / CUCL_GYRO;
    dpsZ = ((float)raw_gyro_z) / CUCL_GYRO;
}

#include <Arduino.h>
#include <Wire.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#define PIN_PWM0 0
int pins[8] = {8, 9, 10, 11, 12, 13, 14, 15};

#define WIDTH 160
#define HEIGHT 120

#define OV7670_ADDR 0x42 // OV7670のI2Cアドレス (書き込み)
uint8_t read_pixel_data();
void writeRegister(uint8_t reg, uint8_t value)
{
    Wire.beginTransmission(OV7670_ADDR >> 1);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

uint8_t frame_buffer[WIDTH * HEIGHT];
#if 0
void capture_image()
{
    uint8_t *ptr = frame_buffer;

    for (int y = 0; y < HEIGHT; y++)
    {
        while (digitalRead(2) == LOW)
            ; // VSYNCの開始を待つ
        while (digitalRead(2) == HIGH)
            ; // VSYNCの終了を待つ

        for (int x = 0; x < WIDTH; x++)
        {
            while (digitalRead(3) == LOW)
                ; // HREFを待つ

            uint8_t y_data = read_pixel_data(); // Yデータ（赤外線情報）
            read_pixel_data();                  // U/Vデータ（不要）

            *ptr++ = y_data;
        }
    }
}
#endif

// #if 1
// void capture_image()
// {
//     uint8_t *ptr = frame_buffer;

//     for (int y = 0; y < HEIGHT; y++)
//     {
//         while (digitalRead(2) == LOW)
//             ; // VSYNCの開始を待つ
//         while (digitalRead(2) == HIGH)
//             ; // VSYNCの終了を待つ

//         for (int x = 0; x < WIDTH; x++)
//         {
//             while (digitalRead(3) == LOW)
//                 ;                     // HREFの開始を待つ
//             *ptr = read_pixel_data(); // ピクセルデータ取得
//             ptr++;
//         }
//     }
// }
// #endif
// // ピクセルデータを取得
// uint8_t read_pixel_data()
// {
//     uint8_t data = 0;
//     for (int i = 0; i < 8; i++)
//     {
//         data |= (digitalRead(pins[i]) << i);
//     }
//     return data;
// }

// void send_image()
// {
//     Serial.write(frame_buffer, sizeof(frame_buffer));
//     Serial.flush();
//     Serial.println("");
//     // for (int i = 0; i < HEIGHT; i++)
//     // {
//     //     for (int j = 0; j < WIDTH; j++)
//     //     {
//     //         Serial.print(frame_buffer[i * WIDTH + j]);
//     //         Serial.print(",");
//     //     }
//     //     Serial.println("");
//     // }
// }
// void setup()
// {
//     /// \tag::setup_pwm[]

//     // Tell GPIO 0 and 1 they are allocated to the PWM
//     gpio_set_function(PIN_PWM0, GPIO_FUNC_PWM);

//     // Find out which PWM slice is connected to GPIO 0 (it's slice 0)
//     uint slice_num = pwm_gpio_to_slice_num(PIN_PWM0);

//     // Set period of 4 cycles (0 to 3 inclusive)
//     pwm_set_wrap(slice_num, 2);
//     // Set channel A output high for one cycle before dropping
//     pwm_set_chan_level(slice_num, PWM_CHAN_A, 1);
//     // Set initial B output high for three cycles before dropping
//     pwm_set_chan_level(slice_num, PWM_CHAN_B, 2);
//     // Set the PWM running
//     pwm_set_enabled(slice_num, true);
//     /// \end::setup_pwm[]

//     // Note we could also use pwm_set_gpio_level(gpio, x) which looks up the
//     // correct slice and channel for a given GPIO.
//     pinMode(25, OUTPUT);
//     digitalWrite(25, HIGH); // LEDを点灯
//     Serial.begin(115200);
//     delay(1000);
//     Serial.println("Hello, world!");
//     Wire.begin();
//     Serial.println("I2c Scanner");
//     for (uint8_t i = 0; i < 128; i++)
//     {
//         Wire.beginTransmission(i);
//         if (Wire.endTransmission() == 0)
//         {
//             Serial.print("Found address: ");
//             Serial.print(i, DEC);
//             Serial.print(" (0x");
//             Serial.print(i, HEX);
//             Serial.println(")");
//         }
//     }
//     Serial.println("Done");
// #if 1
//     writeRegister(0x12, 0x80); // ソフトリセット
//     delay(100);

//     writeRegister(0x12, 0x00); // YUVモードに設定
//     writeRegister(0x11, 0x01); // PCLK設定
//     writeRegister(0x15, 0x20); // HREFを有効化
//     writeRegister(0x11, 0x00); // 内部クロック分周を無効にして最大速度に

//     // ★ 解像度をQQVGA（160x120）に設定
//     writeRegister(0x0C, 0x04); // DCW（ダウンサンプリング）を有効化
//     writeRegister(0x3E, 0x19); // Scaling PCLK control
//     writeRegister(0x70, 0x3A); // Xスケーリング設定
//     writeRegister(0x71, 0x35); // Yスケーリング設定
//     writeRegister(0x72, 0x11); // X subsample control
//     writeRegister(0x73, 0xF1); // Y subsample control
// #endif
// #if 0
//     writeRegister(0x12, 0x80); // ソフトリセット
//     delay(100);
//     writeRegister(0x12, 0x04); // RGBモード
//     writeRegister(0x11, 0x01); // PCLK設定
//     writeRegister(0x15, 0x20); // HREFを有効化
//     writeRegister(0x11, 0x00);  // 内部クロック分周を無効にして最大速度に
// #endif

//     pinMode(2, INPUT); // VSYNC
//     pinMode(3, INPUT); // HREF
//     for (int i = 0; i < 8; i++)
//     {
//         pinMode(pins[i], INPUT);
//     }
//     delay(100);
// }

// void loop()
// {
//     // put your main code here, to run repeatedly:
//     capture_image();
//     send_image();
//     delay(100);
// }

#endif