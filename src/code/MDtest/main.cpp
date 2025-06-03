#ifdef MDTEST
#include <Arduino.h>
#include <Wire.h>
byte b = 0;
void onReceive()
{
    Wire.write(millis() >> 0); // Echo back the received byte
    Wire.write(millis() >> 8);
    Wire.write(millis() >> 16);
    Wire.write(millis() >> 24);
}
void setup()
{
    Serial.begin(115200);
    while (!Serial)
    {
        ; // wait for serial port to connect. Needed for native USB
    }
    Serial.println("MDtest started");
    Wire.swap(); // Use the alternate TWI module
    Wire.begin(0x08);
    Wire.onRequest(onReceive);
}
void loop()
{
    // Your test code here
    Serial.println("Running MDtest...");
    delay(1000); // Delay for 1 second
}
#endif