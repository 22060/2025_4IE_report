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
char buf[8];
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
            buf[i] = CAN.read();
            Serial.print(buf[i], HEX);
            Serial.print(" ");
            buf[i] = buf[i] + 1;
        }
        Serial.println();
        CAN.beginPacket(CAN.packetId());
        for (int i = 0; i < CAN.packetDlc(); i++)
        {
            CAN.write(buf[i]);
        }
        CAN.endPacket();
        Serial.print("Sent CAN Packet ID: ");
        Serial.println(CAN.packetId(), HEX);
        Serial.print("Sent CAN Packet Data: ");
        for (int i = 0; i < CAN.packetDlc(); i++)
        {
            Serial.print(buf[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }
}
#endif