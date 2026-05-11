#include <windows.h>
#include <stdio.h>
#include "input_replay.h"
#include "ui.h"

void UI_LogKeyboardEvent(int index, const INPUT* in)
{
    UI_Log(
        "[%d] KEY | vk=%u | scan=%u | flags=0x%lx | time=%lu",
        index,
        in->ki.wVk,
        in->ki.wScan,
        in->ki.dwFlags,
        in->ki.time
    );
}

void UI_LogMouseEvent(int index, const INPUT* in)
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

    UI_Log(
        "[%-2d] MOUSE | flags=0x%-5lx | x=%ld | y=%ld | data=%lu | time=%lu",
        index,
        in->mi.dwFlags,
        x,
        y,
        in->mi.mouseData,
        in->mi.time
    );
}

void DumpEventFile(const char* filename)
{
    FILE* fp = fopen(filename, "rb");

    if (!fp)
    {
        UI_Log("Failed to open file: %s", filename);
        return;
    }

    int count = 0;

    if (fread(&count, sizeof(int), 1, fp) != 1)
    {
        UI_Log("Failed to read event count");
        fclose(fp);
        return;
    }

    UI_Log("Event count: %d", count);

    INPUT in;

    for (int i = 0; i < count; i++)
    {
        if (fread(&in, sizeof(INPUT), 1, fp) != 1)
        {
            UI_Log("Failed to read event %d", i);
            break;
        }

        switch (in.type)
        {
            case INPUT_KEYBOARD:
                UI_LogKeyboardEvent(i, &in);
                break;

            case INPUT_MOUSE:
                UI_LogMouseEvent(i, &in);
                break;

            default:
                UI_Log("[%d] UNKNOWN INPUT TYPE: %lu", i, in.type);
                break;
        }
    }

    fclose(fp);

    UI_Log("Dump complete.");
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

