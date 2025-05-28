#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <conio.h>
#include <locale.h>
#include <commdlg.h>  // 追加
#include <tchar.h>    // 追加
#include <setupapi.h> // 追加
#ifndef INITGUID
#define INITGUID
#endif
#include <initguid.h>
DEFINE_GUID(GUID_DEVINTERFACE_COMPORT, 0x86E0D1E0, 0x8089, 0x11D0, 0x9C, 0xE4, 0x08, 0x00, 0x3E, 0x30, 0x1F, 0x73);
#pragma comment(lib, "setupapi.lib")
#define MAX_COM_PORTS 32
char port_name[16] = "COM3"; // ユーザー選択用
#define PORT_NAME port_name
#define BAUDRATE CBR_38400

HANDLE hSerial;
volatile int running = 1;
const char *filepath = "yourfile.mot"; // 必要に応じて変更
char filepath_buf[MAX_PATH] = {0};     // 追加
void select_com_port()
{
    HDEVINFO hDevInfo = SetupDiGetClassDevs(
        &GUID_DEVINTERFACE_COMPORT, 0, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (hDevInfo == INVALID_HANDLE_VALUE)
    {
        printf("COMポート一覧取得失敗\n");
        return;
    }
    SP_DEVINFO_DATA devInfoData;
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    char comlist[MAX_COM_PORTS][16];
    int comcount = 0;

    printf("利用可能なCOMポート:\n");
    for (int i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData); i++)
    {
        HKEY hKey = SetupDiOpenDevRegKey(
            hDevInfo, &devInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
        if (hKey)
        {
            char port[16];
            DWORD size = sizeof(port);
            DWORD type = 0;
            if (RegQueryValueExA(hKey, "PortName", NULL, &type, (LPBYTE)port, &size) == ERROR_SUCCESS)
            {
                if (strncmp(port, "COM", 3) == 0 && comcount < MAX_COM_PORTS)
                {
                    strcpy(comlist[comcount], port);
                    printf("  [%d] %s\n", comcount + 1, port);
                    comcount++;
                }
            }
            RegCloseKey(hKey);
        }
    }
    SetupDiDestroyDeviceInfoList(hDevInfo);

    if (comcount == 0)
    {
        printf("COMポートが見つかりませんでした。\n");
        exit(1);
    }
    printf("番号を入力してください: ");
    int sel = 0;
    scanf("%d", &sel);
    if (sel < 1 || sel > comcount)
    {
        printf("不正な番号です。\n");
        exit(1);
    }
    strcpy(port_name, comlist[sel - 1]);
}
unsigned __stdcall recv_thread(void *arg)
{
    char buf[256];
    DWORD bytesRead;
    OVERLAPPED ov = {0};
    ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    while (running)
    {
        ResetEvent(ov.hEvent);
        BOOL ret = ReadFile(hSerial, buf, sizeof(buf) - 1, &bytesRead, &ov);
        if (!ret)
        {
            if (GetLastError() == ERROR_IO_PENDING)
            {
                // 非同期完了待ち
                WaitForSingleObject(ov.hEvent, INFINITE);
                GetOverlappedResult(hSerial, &ov, &bytesRead, FALSE);
            }
            else
            {
                Sleep(10);
                continue;
            }
        }
        if (bytesRead > 0)
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
    CloseHandle(ov.hEvent);
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
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    int count = 0, lines = 0;
    char line[256];
    while (fgets(line, sizeof(line), fp))
        lines++;
    fseek(fp, 0, SEEK_SET);

    OVERLAPPED ov = {0};
    ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    while (fgets(line, sizeof(line), fp))
    {
        count++;
        size_t len = strlen(line);
        if (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
        {
            line[--len] = '\0';
            if (len > 0 && (line[len - 1] == '\r' || line[len - 1] == '\n'))
            {
                line[--len] = '\0';
            }
        }
        char sendbuf[260];
        snprintf(sendbuf, sizeof(sendbuf), "%s\r", line); // CRのみ

        DWORD written = 0;
        ResetEvent(ov.hEvent);
        BOOL ret = WriteFile(hSerial, sendbuf, strlen(sendbuf), &written, &ov);
        if (!ret)
        {
            if (GetLastError() == ERROR_IO_PENDING)
            {
                // 非同期完了待ち
                WaitForSingleObject(ov.hEvent, INFINITE);
                GetOverlappedResult(hSerial, &ov, &written, FALSE);
            }
            else
            {
                printf("WriteFile error\n");
                break;
            }
        }
        if (count % 10 == 0 || count == lines)
        {
            printf("\r%d%%", (int)((double)count / lines * 100));
        }
    }
    CloseHandle(ov.hEvent);
    printf("\n書き込み完了。\n");
    fclose(fp);
}

unsigned __stdcall send_thread(void *arg)
{
    char cmd[256];
    while (running)
    {
        // printf("> ");
        // fflush(stdout);
        if (fgets(cmd, sizeof(cmd), stdin) == NULL)
            break;
        if (cmd[0] == 'l')
        {
            char sendbuf[260];
            snprintf(sendbuf, sizeof(sendbuf), "%s\r", cmd);
            DWORD written;
            OVERLAPPED ov = {0};
            ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
            ResetEvent(ov.hEvent);
            BOOL ret = WriteFile(hSerial, sendbuf, strlen(sendbuf), &written, &ov);
            if (!ret)
            {
                if (GetLastError() == ERROR_IO_PENDING)
                {
                    WaitForSingleObject(ov.hEvent, INFINITE);
                    GetOverlappedResult(hSerial, &ov, &written, FALSE);
                }
                else
                {
                    printf("WriteFile error\n");
                }
            }
            CloseHandle(ov.hEvent);
            send_file(filepath);
        }
        else
        {
            char sendbuf[260];
            snprintf(sendbuf, sizeof(sendbuf), "%s\r", cmd);
            DWORD written;
            OVERLAPPED ov = {0};
            ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
            ResetEvent(ov.hEvent);
            BOOL ret = WriteFile(hSerial, sendbuf, strlen(sendbuf), &written, &ov);
            if (!ret)
            {
                if (GetLastError() == ERROR_IO_PENDING)
                {
                    WaitForSingleObject(ov.hEvent, INFINITE);
                    GetOverlappedResult(hSerial, &ov, &written, FALSE);
                }
                else
                {
                    printf("WriteFile error\n");
                }
            }
            CloseHandle(ov.hEvent);
        }
    }
    running = 0;
    return 0;
}

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "en_US.UTF-8");
    select_com_port(); // ★ここを追加
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
        OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
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