#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

// char *PORT "COM3"      // 使用するシリアルポート
#define BAUDRATE 12500 // ボーレート
HANDLE hSerial;
HANDLE
open_serial_port(const char *port_name)
{
    HANDLE hSerial = CreateFile(
        port_name,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (hSerial == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "❌ シリアルポートエラー: %s\n", port_name);
        return INVALID_HANDLE_VALUE;
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams))
    {
        fprintf(stderr, "❌ シリアルポート設定取得エラー\n");
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }

    dcbSerialParams.BaudRate = BAUDRATE;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams))
    {
        fprintf(stderr, "❌ シリアルポート設定エラー\n");
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }

    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hSerial, &timeouts))
    {
        fprintf(stderr, "❌ シリアルポートタイムアウト設定エラー\n");
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }

    return hSerial;
}

void send_file(HANDLE hSerial, const char *filepath)
{
    printf("🔁 .mot ファイル「%s」を送信中...\n", filepath);
    FILE *file = fopen(filepath, "r");
    if (!file)
    {
        perror("ファイルを開けません");
        return;
    }

    char line[256];
    DWORD bytes_written;
    while (fgets(line, sizeof(line), file))
    {
        // 改行文字を削除
        line[strcspn(line, "\r\n")] = '\0';
        strcat(line, "\r"); // CRを追加
        if (!WriteFile(hSerial, line, strlen(line), &bytes_written, NULL))
        {
            fprintf(stderr, "❌ データ送信エラー\n");
            break;
        }
        printf("送信中: %s\n", line);
    }

    fclose(file);
    printf("書き込み完了。\n");
}
DWORD WINAPI OtherTask(LPVOID lpParam)
{
    HANDLE hSerial = (HANDLE)lpParam; // 受け取った引数を HANDLE に変換

    char cmd[256];
    // 例：シリアルポートに何か書き込む（実際の用途に応じて変更）
    while (1)
    {
        if (!ReadFile(hSerial, cmd, strlen(cmd), &bytes_written, NULL))
        {
            fprintf(stderr, "❌ コマンド受信エラー\n");
        }
        printf("受信: %s\n", cmd);
    }

    return 0;
}

void serial_terminal(HANDLE hSerial, const char *filepath)
{
    printf("\n=== ターミナルモード開始 ===\n");
    printf("Ctrl+C で終了\n\n");

    char cmd[256];
    DWORD bytes_written;

    HANDLE hThread = CreateThread(
        NULL,
        0,
        OtherTask,
        (LPVOID)hSerial, // ← ここで渡す！
        0,
        NULL);

    if (hThread == NULL)
    {
        printf("Failed to create thread\n");
        CloseHandle(hSerial);
        return 1;
    }

    while (1)
    {

        if (fgets(cmd, sizeof(cmd), stdin))
        {
            cmd[strcspn(cmd, "\r\n")] = '\0'; // 改行文字を削除
            if (strcmp(cmd, "l") == 0)
            {
                send_file(hSerial, filepath);
            }
            strcat(cmd, "\r"); // CRを追加
            if (!WriteFile(hSerial, cmd, strlen(cmd), &bytes_written, NULL))
            {
                fprintf(stderr, "❌ コマンド送信エラー\n");
            }
        }
    }
}
void catcher(int sig)
{
    printf("\n\nCtrl+C が押されました。終了します。\n");
    CloseHandle(hSerial);
    exit(0);
}
int main(int argc, char *argv[])
{
    signal(SIGINT, catcher); // Ctrl+Cを無視
    if (argc != 3)
    {
        fprintf(stderr, "使い方: %s yourfile.mot\n", argv[0]);
        return 1;
    }

    const char *filepath = argv[1];
    const char *PORT = argv[2]; // コマンドライン引数からポート名を取得
    while (1)
    {
        hSerial = open_serial_port(PORT);
        if (hSerial == INVALID_HANDLE_VALUE)
        {
            return 1;
        }

        serial_terminal(hSerial, filepath);
    }

    return 0;
}