import serial
import json
import socket

SERIAL_PORT = "COM3"  # Windows
# SERIAL_PORT = "/dev/ttyUSB0"  # Linux
BAUD_RATE = 115200
host = "127.0.0.1"
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
def receive_image():
    with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=5) as ser:
        
        datas = b""
        data = b""
        while(data != b"\r"):
            data = ser.read()
            datas += data
        print(datas)
        # print(json.dumps(json.loads(datas)))
        sock.sendto(datas, (host, 6000))

def main():
    while True:
        receive_image()

if __name__ == "__main__":
    while True:
        try:
            main()
        except KeyboardInterrupt:
            break
        except Exception as e:
            print(e)
            continue
