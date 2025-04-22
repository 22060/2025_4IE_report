import serial
import time
import sys
import threading

PORT = "COM3"
BAUDRATE = 38400  # あなたの環境に合わせて変更
TIMEOUT = 0.1

def send_mot_file(ser, filepath):
    print(f"🔁 .mot ファイル「{filepath}」を送信中...")
    try:
        ser.write(b'l\n')  # Ctrl+C でリセット
        with open(filepath, 'r') as f:
            for line in f:
                line = line.strip()
                if line:
                    print(f"送信中: {line}")  # デバッグ用ログ
                    for i in range(0, len(line), 16):  # 16バイトずつ分割
                        chunk = line[i:i+16]
                        ser.write((chunk).encode())
                        print(f"送信済みチャンク: {chunk}")  # チャンクログ
                        time.sleep(0.5)  # 待機時間を調整
                        # デバイス応答を確認
                        response = ser.read(ser.in_waiting or 1)
                        if response:
                            print(f"デバイス応答: {response.decode(errors='ignore')}")
        print("✅ 転送完了")
    except FileNotFoundError:
        print(f"❌ ファイルが見つかりません: {filepath}")
        sys.exit(1)

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
                send_mot_file(ser, filepath)
            ser.write((cmd + '\r\n').encode())
    except KeyboardInterrupt:
        print("\n🔚 終了します")

def main():
    if len(sys.argv) != 2:
        print("使い方: python flash_and_terminal.py yourfile.mot")
        sys.exit(1)

    filepath = sys.argv[1]

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
