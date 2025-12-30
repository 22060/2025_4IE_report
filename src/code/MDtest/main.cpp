#ifdef MDTEST
#include <Arduino.h>
void setup()
{
    Serial.begin(115200);
    pinMode(33,INPUT);
    pinMode(34,INPUT);
    pinMode(35,INPUT);
    Serial.println("initial fin");
}
void loop()
{
    // Your test code here
    if(digitalRead(33)){
        Serial.print("SW1 pushed");
    }
    if(digitalRead(34)){
        Serial.print("SW2 pushed");
    }
    if(digitalRead(35)){
        Serial.print("SW3 pushed");
    }
    Serial.println();
}
#endif