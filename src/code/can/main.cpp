#ifdef CAN_ESP32
#include <Arduino.h>
#include <CAN.h>

void setup()
{
    Serial.begin(115200);
    Serial.println("CAN ESP32 Example");
    CAN.setPins(4, 5); // TX=GPIO21, RX=GPIO22
    CAN.begin(1000E3); // Initialize CAN at 1 Mbps
}
void loop()
{
    // Your main code here
    if (CAN.parsePacket())
    {
        Serial.print("CAN Packet ID: ");
        Serial.println(CAN.packetId(), HEX);
        Serial.print("CAN Packet Data: ");
        for (int i = 0; i < CAN.packetDlc(); i++)
        {
            Serial.print(CAN.read());
            Serial.print(" ");
        }
        Serial.println();
    }
}
#endif