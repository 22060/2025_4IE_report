#include <Arduino.h>
#include "pid.h"

void pulsea_1();
void pulsea_2();
void pulsea_3();
void pulsea_4();

void pulseb_1();
void pulseb_2();
void pulseb_3();
void pulseb_4();

/* array with three functions */
void (*pid_func_a[])() = { pulsea_1, pulsea_2, pulsea_3, pulsea_4} ;
void (*pid_func_b[])() = { pulseb_1, pulseb_2, pulseb_3, pulseb_4} ;

bool pid::begin(int n,int pin_a,int pin_b,int kp,int ki,int kd){
    if(n >=4 || n<0){
        Serial.println("error:numberは0~3で設定してください");
        return 0;
    }else{
        // pinMode(pin_a,INPUT);
        // pinMode(pin_b,INPUT);
        // attachInterrupt(pin_a,(*pid_func_a[n]),RISING);
        // attachInterrupt(pin_b,(*pid_func_b[n]),RISING);
        pin_setting[n][0] = pin_a;
        pin_setting[n][1] = pin_b;
        Kp[n] = kp;
        Ki[n] = ki;
        Kd[n] = kd;
        for (int i = 0; i < 4; i++)
        {
            old[i] = millis();
        }
        
        return 1;
    }
}

bool pid::begin(int n,int pin_a,int pin_b){
    if(n >=4 || n<0){
        Serial.println("error:numberは0~3で設定してください");
        return 0;
    }else{
        // pinMode(pin_a,INPUT);
        // pinMode(pin_b,INPUT);
        // attachInterrupt(pin_a,(*pid_func_a[n]),RISING);
        // attachInterrupt(pin_b,(*pid_func_b[n]),RISING);
        for (int i = 0; i < 4; i++)
        {
            old[i] = millis();
        }
        return 1;
    }
}
bool pid::k_set(int n,int kp,int ki,int kd){
    if(n >=4 || n<0){
        Serial.println("error:numberは0~3で設定してください");
        return 0;
    }else{
        Kp[n] = kp;
        Ki[n] = ki;
        Kd[n] = kd;
        return 1;
    }
}
void pid::k_set(int kp,int ki,int kd){
    for (int i = 0; i < 4; i++)
    {
        Kp[i] = kp;
        Ki[i] = ki;
        Kd[i] = kd;
    }
}
bool pid::target_set(int n,int value){
    if(n >=4 || n<0){
        Serial.println("error:numberは0~3で設定してください");
        return 0;
    }else{
        target[n] = value;
        return 1;
    }
}
int pid::get_pid(int n,int feed){
    int p,i,d;
    int pidd;
    old_count[n] = feed;
    error[n][0] = error[n][1];
    error[n][1] = target[n]-(old_count[n]-feed);
    integral[n] += (error[n][1] + error[n][0]) / (2.0 * millis()-old[n]);
    p = Kp[n] * error[n][1];
    i = Ki[n] * integral[n];
    d = Kd[n] * (error[n][1] - error[n][0])/millis()-old[n];
    pidd = int(p + i + d);
    old[n] = millis();
    return pidd;
}
int pid::get_pid(int n){
    int p,i,d;
    int pidd;
    int feed = count[n];
    old_count[n] = feed;
    error[n][0] = error[n][1];
    error[n][1] = target[n]-(old_count[n]-feed);
    integral[n] += (error[n][1] + error[n][0]) / (2.0 * millis()-old[n]);
    p = Kp[n] * error[n][1];
    i = Ki[n] * integral[n];
    d = Kd[n] * (error[n][1] - error[n][0])/millis()-old[n];
    pidd = int(p + i + d);
    old[n] = millis();
    return pidd;
}
void pid::reset(int n){
    count[n] = 0;
    old_count[n] = 0;
    error[n][0] = 0;
    error[n][1] = 0;
    integral[n] = 0;
    old[n] = 0;
    target[n] = 0;
}
pid pidctrl;
//ロタコン用関数①
void pulsea_1(){
    if(digitalRead(pidctrl.pin_setting[0][1])){
        pidctrl.count[0]++;
    }else{
        pidctrl.count[0]--;
    }
}
void pulseb_1(){
    if(digitalRead(pidctrl.pin_setting[0][0])){
        pidctrl.count[0]++;
    }else{
        pidctrl.count[0]--;
    }
}
//ロタコン用関数②
void pulsea_2(){
    if(digitalRead(pidctrl.pin_setting[1][1])){
        pidctrl.count[1]++;
    }else{
        pidctrl.count[1]--;
    }
}
void pulseb_2(){
    if(digitalRead(pidctrl.pin_setting[1][0])){
        pidctrl.count[1]++;
    }else{
        pidctrl.count[1]--;
    }
}
//ロタコン用関数③
void pulsea_3(){
    if(digitalRead(pidctrl.pin_setting[2][1])){
        pidctrl.count[2]++;
    }else{
        pidctrl.count[2]--;
    }
}
void pulseb_3(){
    if(digitalRead(pidctrl.pin_setting[2][0])){
        pidctrl.count[2]++;
    }else{
        pidctrl.count[2]--;
    }
}
//ロタコン用関数④
void pulsea_4(){
    if(digitalRead(pidctrl.pin_setting[3][1])){
        pidctrl.count[3]++;
    }else{
        pidctrl.count[3]--;
    }
}
void pulseb_4(){
    if(digitalRead(pidctrl.pin_setting[3][0])){
        pidctrl.count[3]++;
    }else{
        pidctrl.count[3]--;
    }
}