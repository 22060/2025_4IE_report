import serial
import time
import sys

def send_file(ser, filepath):
    print(f"🔁 .mot ファイル「{filepath}」を送信中...")
    ser.flushInput()  # 入力バッファをクリア
    ser.write(b'l\r')  # Ctrl+C でリセット
    with open(filepath, 'r') as f:
        for line in f:
            line = line.strip()  # 改行文字削除
            ser.write((line + '\r').encode('ascii'))  # CRだけ送る
            ser.flush()  # 念のため送信バッファ強制書き出し
    ser.write(b'g\r')  # Ctrl+C でリセット

def main():
    port = 'COM3'  # 使用しているポートに合わせて変更
    baudrate = 38400  # ボーレートはマイコンに合わせて変更
    filepath = sys.argv[1]

    try:
        with serial.Serial(port, baudrate, timeout=1) as ser:
            print(f"Connected to {port} at {baudrate} baud.")
            input("マイコンをブートモードにしてリセットし、Enterキーを押してください...")
            send_file(ser, filepath)
            print("書き込み完了。")
    except serial.SerialException as e:
        print(f"シリアルポートエラー: {e}")

if __name__ == "__main__":
    main()
