#include <SimpleFOC.h>

// ===== モーター・ドライバ =====
BLDCMotor motor = BLDCMotor(7);
BLDCDriver6PWM driver = BLDCDriver6PWM(A_PHASE_UH, A_PHASE_UL, A_PHASE_VH, A_PHASE_VL, A_PHASE_WH, A_PHASE_WL);
LowsideCurrentSense currentSense = LowsideCurrentSense(0.003f, -64.0f / 7.0f, A_OP1_OUT, A_OP2_OUT, A_OP3_OUT);

// ===== エンコーダ =====
Encoder encoder = Encoder(A_HALL1, A_HALL2, 2048, A_HALL3);
void doA() { encoder.handleA(); }
void doB() { encoder.handleB(); }
void doIndex() { encoder.handleIndex(); }

// ===== コマンダ =====
Commander command = Commander(Serial);
void doTarget(char *cmd) { command.motion(&motor, cmd); }

void setup()
{
    // ===== センサ =====
    encoder.init();
    encoder.enableInterrupts(doA, doB, doIndex);
    motor.linkSensor(&encoder);

    // ===== ドライバ =====
    driver.voltage_power_supply = 16;
    driver.init();
    motor.linkDriver(&driver);

    // ===== 電流検出 =====
    currentSense.linkDriver(&driver);
    currentSense.init();
    currentSense.skip_align = true;
    motor.linkCurrentSense(&currentSense);

    // ===== 基本設定 =====
    motor.voltage_sensor_align = 1;
    motor.velocity_index_search = 3;

    // 🔥 電圧フルに使う
    motor.voltage_limit = 15;   // 電源いっぱいまで使う

    // 🔥 速度制御
    motor.controller = MotionControlType::velocity;
    motor.torque_controller = TorqueControlType::foc_current;

    // 🔥 制限を全部開放気味に
    motor.current_limit = 10;      // 要注意（様子見ながら）
    motor.velocity_limit = 10000;  // 実質無制限

    // ===== 電流PID =====
    motor.PID_current_q.P = motor.PID_current_d.P = 0.1;
    motor.PID_current_q.I = motor.PID_current_d.I = 10;

    // ===== 速度PID（強め） =====
    motor.PID_velocity.P = 0.5;
    motor.PID_velocity.I = 10;
    motor.PID_velocity.output_ramp = 0;   // 加速制限なし
    motor.LPF_velocity.Tf = 0.005;

    // ===== 通信 =====
    Serial.begin(115200);
    motor.useMonitoring(Serial);

    motor.init();
    motor.initFOC();

    command.add('T', doTarget, "target velocity");

    Serial.println("Motor ready.");
    Serial.println("Send: T<velocity rad/s>");
    delay(1000);
}

void loop()
{
    motor.loopFOC();
    motor.move();
    command.run();
}