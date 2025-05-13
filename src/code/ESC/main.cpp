#ifdef ESC1
// #include <SimpleFOC.h>

// BLDCMotor motor = BLDCMotor(7);
// BLDCDriver6PWM driver = BLDCDriver6PWM(A_PHASE_UH, A_PHASE_UL, A_PHASE_VH, A_PHASE_VL, A_PHASE_WH, A_PHASE_WL);
// LowsideCurrentSense currentSense = LowsideCurrentSense(0.003f, -64.0f / 7.0f, A_OP1_OUT, A_OP2_OUT, A_OP3_OUT);

// Encoder encoder = Encoder(A_HALL1, A_HALL2, 2048, A_HALL3);

// void doA() { encoder.handleA(); }
// void doB() { encoder.handleB(); }
// void doIndex() { encoder.handleIndex(); }

// Commander command = Commander(Serial);
// void doTarget(char *cmd) { command.motion(&motor, cmd); }

#include <SimpleFOC.h>

// ===========================
// ユーザー定義パラメータ
// ===========================
// モーターのパラメータ
#define MOTOR_POLE_PAIRS 7        // 極対数
#define VOLTAGE_POWER_SUPPLY 16.0 // 電源電圧 [V]
#define VOLTAGE_LIMIT 6.0         // 最大印加電圧 [V]

// 制御ループ設定
#define TARGET_VELOCITY 20.0 // 回転速度 [rad/s]

// ===========================
// モーター・ドライバ初期化
// ===========================
BLDCMotor motor = BLDCMotor(MOTOR_POLE_PAIRS);
BLDCDriver6PWM driver = BLDCDriver6PWM(
    A_PHASE_UH, A_PHASE_UL,
    A_PHASE_VH, A_PHASE_VL,
    A_PHASE_WH, A_PHASE_WL);

// センサレス用ベロシティオープンループ制御
void setup()
{
    Serial.begin(115200);

    // ドライバ設定
    driver.voltage_power_supply = VOLTAGE_POWER_SUPPLY;
    driver.voltage_limit = VOLTAGE_LIMIT;
    driver.init();
    motor.linkDriver(&driver);

    // モーター設定
    motor.controller = MotionControlType::velocity_openloop;

    motor.init();

    Serial.println("SimpleFOC センサレス BLDC 初期化完了");
}

// 制御ループ
void loop()
{
    motor.move(1.0);
}
#endif