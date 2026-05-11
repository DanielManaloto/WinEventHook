#include "ui.h"
#include "file_handler.h"

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <conio.h>

static HANDLE hOut;

#define LOG_START_ROW 15
#define LOG_END_ROW   25
#define LOG_WIDTH     70
#define SELECTOR_VISIBLE_ROWS 15

static int logRow = LOG_START_ROW;

void UI_Init()
{
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    if (hOut == INVALID_HANDLE_VALUE)
        return;

    DWORD mode = 0;

    if (GetConsoleMode(hOut, &mode)) {

        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

        SetConsoleMode(hOut, mode);
    }

    COORD size = {120, 40};
    SetConsoleScreenBufferSize(hOut, size);

    SMALL_RECT rect = {0, 0, 119, 39};
    SetConsoleWindowInfo(hOut, TRUE, &rect);

    // Disable cursor blinking
    CONSOLE_CURSOR_INFO cci;
    GetConsoleCursorInfo(hOut, &cci);

    cci.bVisible = FALSE;

    SetConsoleCursorInfo(hOut, &cci);
}

void UI_Clear()
{
    printf("\x1b[2J");
    printf("\x1b[H");
}

void UI_SetColor(UI_Color color)
{
    switch (color) {

        case UI_COLOR_RED:
            printf("\x1b[31m");
            break;

        case UI_COLOR_GREEN:
            printf("\x1b[32m");
            break;

        case UI_COLOR_YELLOW:
            printf("\x1b[33m");
            break;

        case UI_COLOR_BLUE:
            printf("\x1b[34m");
            break;

        case UI_COLOR_MAGENTA:
            printf("\x1b[35m");
            break;

        case UI_COLOR_CYAN:
            printf("\x1b[36m");
            break;

        case UI_COLOR_WHITE:
            printf("\x1b[37m");
            break;

        default:
            printf("\x1b[0m");
            break;
    }
}


void UI_MoveCursor(int row, int col)
{
    printf("\x1b[%d;%dH", row, col);
}

void UI_DrawLine(int width)
{
    for (int i = 0; i < width; i++)
        printf("=");

    printf("\n");
}

void UI_DrawTitle(const char* title)
{
    UI_SetColor(UI_COLOR_GREEN);

    UI_DrawLine(70);

    printf("                                %s\n", title);

    UI_DrawLine(70);

    UI_SetColor(UI_COLOR_DEFAULT);
}

void UI_DrawMenu()
{
    UI_Clear();
    printf("\n");

    UI_SetColor(UI_COLOR_CYAN);

    UI_DrawTitle("WinEventHook");

    UI_SetColor(UI_COLOR_DEFAULT);

    printf("\n");

    printf("[F8]  View Recording\n");
    printf("[F9]  Start/Stop Recording\n");
    printf("[F10] Replay Recording\n");
    printf("[F11] Exit\n");

    printf("\n");

    UI_MoveCursor(11, 0);

    UI_DrawLine(70);

    UI_MoveCursor(13, 0);

    UI_DrawLine(70);

    fflush(stdout);
}

void DrawStatus(const char* status, UI_Color color)
{
    UI_MoveCursor(12, 2);

    printf("\033[K");
    printf("Status:");

    UI_MoveCursor(12, 10);

    UI_SetColor(color);

    printf("%s", status);

    UI_SetColor(UI_COLOR_DEFAULT);

    fflush(stdout);
}

void UI_Log(const char* fmt, ...)
{
    char buffer[512];

    va_list args;

    va_start(args, fmt);

    vsnprintf(
        buffer,
        sizeof(buffer),
        fmt,
        args
    );

    va_end(args);

    if (logRow > LOG_END_ROW) {

        for (int r = LOG_START_ROW; r <= LOG_END_ROW; r++) {

            UI_MoveCursor(r, 2);

            for (int i = 0; i < LOG_WIDTH; i++)
                printf(" ");
        }

        logRow = LOG_START_ROW;
    }

    UI_MoveCursor(logRow, 2);

    UI_SetColor(UI_COLOR_CYAN);

    printf("[LOG] %s", buffer);

    UI_SetColor(UI_COLOR_DEFAULT);

    fflush(stdout);

    logRow++;
}

const char *UI_SelectReplayFile(const char *dir_path)
{
    int count = 0;
    char **files = list_files(dir_path, &count);

    if (!files || count == 0) {
        UI_Clear();

        UI_SetColor(UI_COLOR_RED);
        printf("\n  No recording files found in '%s'!\n", dir_path);
        printf("  Press any key to return...\n");
        UI_SetColor(UI_COLOR_DEFAULT);

        _getch();

        if (files)
            free_file_list(files, count);

        UI_DrawMenu();
        return NULL;
    }

    int selectedIndex = 0;
    static char selectedFile[MAX_PATH];

    UI_Clear();

    // HOTKEY IDS 
    #define HK_UP      1
    #define HK_DOWN    2
    #define HK_ENTER   3
    #define HK_ESC     4

    RegisterHotKey(NULL, HK_UP,    0, VK_UP);
    RegisterHotKey(NULL, HK_DOWN,  0, VK_DOWN);
    RegisterHotKey(NULL, HK_ENTER, 0, VK_RETURN);
    RegisterHotKey(NULL, HK_ESC,   0, VK_ESCAPE);

    MSG msg;

    while (1) {

        UI_MoveCursor(1, 1);

        UI_DrawTitle("Select a Recording");

        UI_SetColor(UI_COLOR_YELLOW);

        printf("\n  [UP/DOWN] Navigate   [ENTER] Select and Start Replay   [ESC] Cancel    \n");

        UI_SetColor(UI_COLOR_DEFAULT);

        int startIdx = 0;
        int endIdx = count;

        if (count > SELECTOR_VISIBLE_ROWS) {
            startIdx = selectedIndex - (SELECTOR_VISIBLE_ROWS / 2);

            if (startIdx < 0)
                startIdx = 0;

            endIdx = startIdx + SELECTOR_VISIBLE_ROWS;

            if (endIdx > count) {
                endIdx = count;
                startIdx = count - SELECTOR_VISIBLE_ROWS;

                if (startIdx < 0)
                    startIdx = 0;
            }
        }

        for (int i = startIdx; i < endIdx; i++) {
            if (i == selectedIndex) {
                UI_SetColor(UI_COLOR_GREEN);
                printf("  -> %-60s\n", files[i]);
                UI_SetColor(UI_COLOR_DEFAULT);
            }
            else {
                printf("     %-60s\n", files[i]);
            }
        }

        // WAIT FOR HOTKEY 
        if (GetMessage(&msg, NULL, 0, 0)) {

            if (msg.message == WM_HOTKEY) {

                switch (msg.wParam) {

                    case HK_UP:
                        selectedIndex--;

                        if (selectedIndex < 0)
                            selectedIndex = count - 1;
                        break;

                    case HK_DOWN:
                        selectedIndex++;

                        if (selectedIndex >= count)
                            selectedIndex = 0;
                        break;

                    case HK_ENTER:
                        strncpy(selectedFile, files[selectedIndex], MAX_PATH - 1);
                        selectedFile[MAX_PATH - 1] = '\0';

                        goto cleanup_return;

                    case HK_ESC:
                        goto cleanup_cancel;

                }
            }
        }
    }

cleanup_return:

    UnregisterHotKey(NULL, HK_UP);
    UnregisterHotKey(NULL, HK_DOWN);
    UnregisterHotKey(NULL, HK_ENTER);
    UnregisterHotKey(NULL, HK_ESC);

    free_file_list(files, count);

    UI_DrawMenu();

    return selectedFile;

cleanup_cancel:

    UnregisterHotKey(NULL, HK_UP);
    UnregisterHotKey(NULL, HK_DOWN);
    UnregisterHotKey(NULL, HK_ENTER);
    UnregisterHotKey(NULL, HK_ESC);

    free_file_list(files, count);

    UI_DrawMenu();

    return NULL;
}