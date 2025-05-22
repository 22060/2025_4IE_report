import serial
import time
import sys
import threading
import os

PORT = "COM3"
BAUDRATE = 38400  # あなたの環境に合わせて変更
TIMEOUT = 0.1

def send_file(ser, filepath):
    print(f"🔁 .mot ファイル「{filepath}」を送信中...")
    print("persent:",end='')
    ser.flushInput()  # 入力バッファをクリア
    ser.write(b'l\r')  # Ctrl+C でリセット
    with open(filepath, 'r') as f:
        size = sum(1 for _ in f)  # ファイルの行数をカウント
        count = 0
        f.seek(0)  # ファイルポインタを先頭に戻す
        for line in f:
            count += 1
            line = line.strip()  # 改行文字削除
            ser.write((line + '\r').encode('ascii'))  # CRだけ送る
            ser.flush()  # 念のため送信バッファ強制書き出し
            print(f"\r{int(count / size * 100)}%", end='', flush=True)
    print("書き込み完了。")

def serial_terminal(ser, filepath):
    def read_thread():
        while True:
            try:
                data = ser.read(ser.in_waiting or 1)
                if data:
                    print(data.decode(errors="ignore"), end='', flush=True)
            except serial.SerialException:
                break

    threading.Thread(target=read_thread, daemon=True).start()

    print("\n=== ターミナルモード開始 ===")
    print("Ctrl+C で終了\n")
    try:
        while True:
            cmd = input()
            if(cmd == 'l'):
                send_file(ser, filepath)
            ser.write((cmd + '\r').encode())
    except KeyboardInterrupt:
        print("\n🔚 終了します")

def main():
    if len(sys.argv) != 2:
        print("使い方: python flash_and_terminal.py yourfile.mot")
        sys.exit(1)

    filepath = sys.argv[1]
    while True:
        try:
            with serial.Serial(PORT, BAUDRATE, timeout=TIMEOUT) as ser:
                time.sleep(2)  # ポートの安定化待ち
                # send_mot_file(ser, filepath)
                serial_terminal(ser, filepath)
        except serial.SerialException as e:
            print(f"❌ シリアルポートエラー: {e}")
            sys.exit(1)

if __name__ == "__main__":
    main()
