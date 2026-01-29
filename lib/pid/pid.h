#ifndef PIDLIB
#define PIDLIB
#include <Arduino.h>
class pid
{
public:
    bool begin(int n, int pin_a, int pin_b, int kp, int ki, int kd);
    bool begin(int n, int pin_a, int pin_b);
    void k_set(int kp, int ki, int kd);
    bool k_set(int n, int kp, int ki, int kd);
    int get_pid(int n, int feed);
    int get_pid(int n);
    bool target_set(int n, int value);
    void reset(int n);
    int old_count[4];
    int pin_setting[4][2];
    int count[4];
    int Kp[4], Ki[4], Kd[4];
    int number[4];
    int error[4][2];
    int integral[4];
    int old[4];
    int target[4];
};
#endif
