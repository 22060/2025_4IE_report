#ifdef SERIAL
#include <Arduino.h>

void setup()
{
    Serial.begin(115200);
    delay(100);
    Serial.println("Serial Test Start");
    Serial1.begin(115200);
}

void loop()
{
    if (Serial1.available())
    {
        String receivedData = Serial1.readStringUntil('\n');
        // Serial.print("Received from Serial1: ");
        Serial.println(receivedData);
    }

    if (Serial.available())
    {
        int byteToSend = Serial.read();
        Serial1.write(byteToSend);
        Serial.print("Sent to Serial1: ");
        Serial.println(byteToSend, DEC);
    }
}


#endif