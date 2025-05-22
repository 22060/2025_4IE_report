#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <conio.h>
#include <locale.h>
#include <commdlg.h> // 追加

#define PORT_NAME "\\\\.\\COM3"
#define BAUDRATE CBR_38400

HANDLE hSerial;
volatile int running = 1;
const char *filepath = "yourfile.mot"; // 必要に応じて変更
char filepath_buf[MAX_PATH] = {0};     // 追加

unsigned __stdcall recv_thread(void *arg)
{
    char buf[256];
    DWORD bytesRead;
    while (running)
    {
        if (ReadFile(hSerial, buf, sizeof(buf) - 1, &bytesRead, NULL) && bytesRead > 0)
        {
            buf[bytesRead] = '\0';
            printf("%s", buf);
            fflush(stdout);
        }
        else
        {
            Sleep(10);
        }
    }
    return 0;
}

void send_file(const char *filepath)
{
    FILE *fp = fopen(filepath, "r");
    if (!fp)
    {
        printf("ファイルオープン失敗: %s\n", filepath);
        return;
    }
    printf("[INFO] .mot ファイル「%s」を送信中...\n", filepath);
    printf("persent:");
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    int count = 0, lines = 0;
    char line[256];
    // 行数カウント
    while (fgets(line, sizeof(line), fp))
        lines++;
    fseek(fp, 0, SEEK_SET);

    while (fgets(line, sizeof(line), fp))
    {
        count++;
        char *p = line;
        while (*p && (*p == '\r' || *p == '\n'))
            p++; // 改行除去
        char sendbuf[260];
        snprintf(sendbuf, sizeof(sendbuf), "%s\r", p);
        DWORD written;
        WriteFile(hSerial, sendbuf, strlen(sendbuf), &written, NULL);
        printf("\r%d%%", (int)((double)count / lines * 100));
        fflush(stdout);
        Sleep(1); // 必要に応じて調整
    }
    printf("\n書き込み完了。\n");
    fclose(fp);
}

unsigned __stdcall send_thread(void *arg)
{
    char cmd[256];
    while (running)
    {
        printf("> ");
        fflush(stdout);
        if (fgets(cmd, sizeof(cmd), stdin) == NULL)
            break;
        if (cmd[0] == 'l')
        {
            send_file(filepath);
        }
        char sendbuf[260];
        snprintf(sendbuf, sizeof(sendbuf), "%s\r", cmd);
        DWORD written;
        WriteFile(hSerial, sendbuf, strlen(sendbuf), &written, NULL);
    }
    running = 0;
    return 0;
}

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "en_US.UTF-8");
    // ファイル選択ダイアログ
    if (argc != 2)
    {
        OPENFILENAME ofn;
        ZeroMemory(&ofn, sizeof(ofn));
        char initial_dir[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, initial_dir);
        ofn.lpstrInitialDir = initial_dir;
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = filepath_buf;
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = sizeof(filepath_buf);
        ofn.lpstrFilter = "MOT Files (*.mot)\0*.mot\0All Files (*.*)\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        printf("ファイルを選択してください...\n");
        if (GetOpenFileName(&ofn) == FALSE)
        {
            printf("ファイルが選択されませんでした。\n何かキーを押すと終了します...\n");
            getchar();
            return 1;
        }
        filepath = filepath_buf;
    }
    else
    {
        filepath = argv[1];
    }

    hSerial = CreateFileA(
        PORT_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL,
        OPEN_EXISTING, 0, NULL);
    if (hSerial == INVALID_HANDLE_VALUE)
    {
        printf("[ERROR] シリアルポートオープン失敗\n");
        printf("何かキーを押すと終了します...\n");
        getchar();
        return 1;
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    GetCommState(hSerial, &dcbSerialParams);
    dcbSerialParams.BaudRate = BAUDRATE;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    SetCommState(hSerial, &dcbSerialParams);

    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 10;
    timeouts.ReadTotalTimeoutConstant = 10;
    timeouts.ReadTotalTimeoutMultiplier = 1;
    SetCommTimeouts(hSerial, &timeouts);

    HANDLE hRecv = (HANDLE)_beginthreadex(NULL, 0, recv_thread, NULL, 0, NULL);
    HANDLE hSend = (HANDLE)_beginthreadex(NULL, 0, send_thread, NULL, 0, NULL);

    WaitForSingleObject(hSend, INFINITE);
    running = 0;
    WaitForSingleObject(hRecv, INFINITE);

    CloseHandle(hSerial);
    printf("何かキーを押すと終了します...\n");
    getchar();
    return 0;
}