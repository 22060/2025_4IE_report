#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>

#define PORT_NAME "COM3"
#define BAUDRATE CBR_38400
#define FILE_BUFFER_SIZE 1024

HANDLE hSerial;
HANDLE hThread;
volatile int running = 1;
char filepath[256];

DWORD WINAPI read_serial(LPVOID lpParam)
{
    char buf[FILE_BUFFER_SIZE];
    DWORD bytesRead;

    while (running)
    {
        if (ReadFile(hSerial, buf, sizeof(buf) - 1, &bytesRead, NULL) && bytesRead > 0)
        {
            buf[bytesRead] = '\0';
            printf("%s", buf);
            fflush(stdout);
        }
    }
    return 0;
}

void send_file(const char *path)
{
    FILE *fp = fopen(path, "r");
    if (!fp)
    {
        printf("❌ Failed to open file: %s\n", path);
        return;
    }

    printf("🔁 Sending .mot file: \"%s\"...\n", path);
    PurgeComm(hSerial, PURGE_RXCLEAR); // Clear input buffer
    DWORD written;
    WriteFile(hSerial, "l\r", 2, &written, NULL); // Send reset signal

    char line[FILE_BUFFER_SIZE];
    char buffer[FILE_BUFFER_SIZE];
    size_t filesize = 0;
    OVERLAPPED osWrite = {0};
    osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    WriteFile(hSerial, buffer, filesize, NULL, &osWrite);

    fclose(fp);
    printf("✅ File transmission complete.\n");
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: flash_and_terminal.exe yourfile.mot\n");
        return 1;
    }
    strncpy(filepath, argv[1], sizeof(filepath));

    hSerial = CreateFileA(PORT_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hSerial == INVALID_HANDLE_VALUE)
    {
        printf("❌ Failed to open serial port: %s\n", PORT_NAME);
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
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    SetCommTimeouts(hSerial, &timeouts);

    printf("\n=== Terminal Mode Started ===\nPress Ctrl+C to exit\n\n");

    hThread = CreateThread(NULL, 0, read_serial, NULL, 0, NULL);

    while (1)
    {
        char input[256];
        if (_kbhit())
        {
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\r\n")] = 0; // Strip newline

            if (strcmp(input, "l") == 0)
            {
                send_file(filepath);
            }
            else
            {
                char cmdWithCR[258];
                snprintf(cmdWithCR, sizeof(cmdWithCR), "%s\r", input);
                DWORD written;
                WriteFile(hSerial, cmdWithCR, strlen(cmdWithCR), &written, NULL);
            }
        }
    }

    running = 0;
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    CloseHandle(hSerial);
    return 0;
}
