import serial
import numpy as np
import cv2

SERIAL_PORT = "COM11"  # Windows
# SERIAL_PORT = "/dev/ttyUSB0"  # Linux
BAUD_RATE = 115200
WIDTH = 160
HEIGHT = 120

def receive_image():
    with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=5) as ser:
        print("Waiting for IR image data...")
        
        data = ser.read(WIDTH * HEIGHT)
        print(data)
        if len(data) != WIDTH * HEIGHT:
            print("Incomplete image received")
            return None

        img = np.frombuffer(data, dtype=np.uint8).reshape((HEIGHT, WIDTH))
        return img

def main():
    while True:
        img = receive_image()
        if img is not None:
            cv2.imshow("Infrared Image", img)
            cv2.waitKey(1)  # 1ms待機（リアルタイム表示）

if __name__ == "__main__":
    while True:
        try:
            main()
        except KeyboardInterrupt:
            break
        except Exception as e:
            print(e)
            continue
