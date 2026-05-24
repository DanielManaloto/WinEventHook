#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include "input_replay.h"
#include "ui.h"

#define SELECTOR_VISIBLE_ROWS 30

void FormatEventString(char* buffer, size_t max_len, int index, const INPUT* in)
{
    if (in->type == INPUT_KEYBOARD)
    {
        snprintf(buffer, max_len, "[%-4d] KEY   | vk=%-3u | scan=%-3u | flags=0x%-4lx | time=%lu",
                 index,
                 in->ki.wVk,
                 in->ki.wScan,
                 in->ki.dwFlags,
                 in->ki.time);
    }
    else if (in->type == INPUT_MOUSE)
    {
        LONG x = in->mi.dx;
        LONG y = in->mi.dy;

        if (in->mi.dwFlags & MOUSEEVENTF_ABSOLUTE)
        {
            int screenW = GetSystemMetrics(SM_CXSCREEN);
            int screenH = GetSystemMetrics(SM_CYSCREEN);

            x = (in->mi.dx * screenW) / 65535;
            y = (in->mi.dy * screenH) / 65535;
        }

        snprintf(buffer, max_len, "[%-4d] MOUSE | flags=0x%-4lx | x=%-5ld | y=%-5ld | data=%-3lu | time=%lu",
                 index,
                 in->mi.dwFlags,
                 x,
                 y,
                 in->mi.mouseData,
                 in->mi.time);
    }
    else
    {
        snprintf(buffer, max_len, "[%-4d] UNKNOWN INPUT TYPE: %lu", index, in->type);
    }
}

void DumpEventFile(const char* filename)
{
    FILE* fp = fopen(filename, "rb");

    if (!fp)
    {
        UI_Clear();
        UI_SetColor(UI_COLOR_RED);
        printf("\n  Failed to open file: %s\n", filename);
        printf("  Press any key to return...\n");
        UI_SetColor(UI_COLOR_DEFAULT);
        _getch();
        UI_DrawMenu();
        return;
    }

    int count = 0;

    if (fread(&count, sizeof(int), 1, fp) != 1 || count <= 0)
    {
        UI_Clear();
        UI_SetColor(UI_COLOR_RED);
        printf("\n  Failed to read event count or file is empty.\n");
        printf("  Press any key to return...\n");
        UI_SetColor(UI_COLOR_DEFAULT);
        fclose(fp);
        _getch();
        UI_DrawMenu();
        return;
    }

    // Allocate memory and read all events at once
    INPUT* events = (INPUT*)malloc(count * sizeof(INPUT));
    if (!events)
    {
        fclose(fp);
        return;
    }

    if (fread(events, sizeof(INPUT), count, fp) != (size_t)count){
        // Handle partial read
    }
    fclose(fp);

    int selectedIndex = 0;

    // HOTKEY IDS
    #define HK_UP    1
    #define HK_DOWN  2
    #define HK_ESC   4

    RegisterHotKey(NULL, HK_UP,   0, VK_UP);
    RegisterHotKey(NULL, HK_DOWN, 0, VK_DOWN);
    RegisterHotKey(NULL, HK_ESC,  0, VK_ESCAPE);

    MSG msg;

    while (1) {
        UI_Clear();
        UI_MoveCursor(1, 1);

        UI_SetColor(UI_COLOR_YELLOW);
        printf("\n  [UP/DOWN] Navigate   [ESC] Exit Viewer    (Total Events: %d)\n\n", count);
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

        // Draw the events
        for (int i = startIdx; i < endIdx; i++) {
            char lineBuffer[256];
            FormatEventString(lineBuffer, sizeof(lineBuffer), i, &events[i]);

            if (i == selectedIndex) {
                UI_SetColor(UI_COLOR_GREEN);
                printf("  -> %-80s\n", lineBuffer);
                UI_SetColor(UI_COLOR_DEFAULT);
            }
            else {
                printf("     %-80s\n", lineBuffer);
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

                    case HK_ESC:
                        goto cleanup_exit;
                }
            }
        }
    }

cleanup_exit:
    UnregisterHotKey(NULL, HK_UP);
    UnregisterHotKey(NULL, HK_DOWN);
    UnregisterHotKey(NULL, HK_ESC);

    free(events);
    UI_DrawMenu();
}

#define REPLAY_SPEED 0.8f
void Replay(const char *filename){

    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        printf("Failed to open file: %s\n", filename);
        return;
    }

    int count = 0;

    if (fread(&count, sizeof(int), 1, fp) != 1) {
        printf("Failed to read event count\n");
        fclose(fp);
        return;
    }

    printf("Event count: %d\n\n", count);
    INPUT in;
    DWORD lastTime = 0;
    DWORD delay = 10;
    for (int i = 0; i < count; i++) {
        in = (INPUT){0};
        if (fread(&in, sizeof(INPUT), 1, fp) != 1) {
            printf("Failed to read event %d\n", i);
            break;
        }
        
        if (in.type == INPUT_KEYBOARD) {
            delay = in.ki.time - lastTime;
            lastTime = in.ki.time;
            in.ki.time = 0;
        }

        else if (in.type == INPUT_MOUSE) {
            delay = in.mi.time - lastTime;
            lastTime = in.mi.time;
            in.mi.time = 0;
        }

        Sleep(delay * REPLAY_SPEED);
        SendInput(1, &in, sizeof(INPUT));
    }
    fclose(fp);
}

