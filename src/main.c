#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ui.h"
#include "input_recorder.h"
#include "input_replay.h"

#define HOTKEY_VIEW   VK_F8
#define HOTKEY_TOGGLE VK_F9
#define HOTKEY_REPLAY VK_F10
#define HOTKEY_EXIT   VK_F11

#define EVENTS_DIR "data"

enum {
    HK_VIEW = 1,
    HK_TOGGLE,
    HK_REPLAY,
    HK_EXIT
};

static void DisableHotkeys() {
    UnregisterHotKey(NULL, HK_VIEW);
    UnregisterHotKey(NULL, HK_TOGGLE);
    UnregisterHotKey(NULL, HK_REPLAY);
    //UnregisterHotKey(NULL, HK_EXIT);
}

static void EnableHotkeys() {
    RegisterHotKey(NULL, HK_VIEW, 0, HOTKEY_VIEW);
    RegisterHotKey(NULL, HK_TOGGLE, 0, HOTKEY_TOGGLE);
    RegisterHotKey(NULL, HK_REPLAY, 0, HOTKEY_REPLAY);
    //RegisterHotKey(NULL, HK_EXIT, 0, HOTKEY_EXIT);
}

int main() {
    InputRecorder rec;
    MSG msg;

    UI_Init();
    UI_DrawMenu();

    if (!IR_Init(&rec)) {
        DrawStatus("Recorder Init Failed", UI_COLOR_RED);
        return 1;
    }

    EnableHotkeys();
    RegisterHotKey(NULL, HK_EXIT, 0, HOTKEY_EXIT);

    DrawStatus("Idle", UI_COLOR_GREEN);

    while (GetMessage(&msg, NULL, 0, 0)) {

        if (msg.message == WM_HOTKEY) {

            switch (msg.wParam) {

                case HK_VIEW: {
                    
                    DisableHotkeys();

                    const char *file = UI_SelectReplayFile(EVENTS_DIR);

                    DrawStatus("Viewing...", UI_COLOR_CYAN);

                    DumpEventFile(file);
                    DrawStatus("Idle", UI_COLOR_CYAN);

                    EnableHotkeys();

                    break;

                }

                case HK_TOGGLE:

                    UnregisterHotKey(NULL, HK_VIEW);
                    UnregisterHotKey(NULL, HK_REPLAY);

                    if (!rec.recording) {

                        IR_Start(&rec);

                        DrawStatus(
                            "Recording...",
                            UI_COLOR_YELLOW
                        );

                    } else {

                        IR_Stop(&rec);

                        char filename[MAX_PATH];

                        SYSTEMTIME st;
                        GetLocalTime(&st);

                        snprintf(
                            filename,
                            MAX_PATH,
                            EVENTS_DIR "\\events_%04d%02d%02d_%02d%02d%02d.bin",
                            st.wYear,
                            st.wMonth,
                            st.wDay,
                            st.wHour,
                            st.wMinute,
                            st.wSecond
                        );

                        IR_SaveToFile(&rec, filename);

                        DrawStatus("Saved", UI_COLOR_GREEN);

                        UI_MoveCursor(12, 15);

                        printf(
                            " %d events -> %s\n",
                            rec.count,
                            filename
                        );

                        RegisterHotKey(NULL, HK_VIEW, 0, HOTKEY_VIEW);
                        RegisterHotKey(NULL, HK_REPLAY, 0, HOTKEY_REPLAY);
                    }

                    break;

                case HK_REPLAY: {

                    DisableHotkeys();

                    const char *file = UI_SelectReplayFile(EVENTS_DIR);
                    
                    if (!file) break;

                    DrawStatus("Replaying...", UI_COLOR_CYAN);

                    Replay(file);

                    DrawStatus("Idle", UI_COLOR_GREEN);

                    EnableHotkeys();
                    RegisterHotKey(NULL, HK_EXIT, 0, HOTKEY_EXIT);

                    break;
                }

                case HK_EXIT:

                    DrawStatus("Exiting...", UI_COLOR_RED);

                    goto cleanup;
            }
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

cleanup:

    DisableHotkeys();
    UnregisterHotKey(NULL, HK_EXIT);

    IR_Shutdown(&rec);

    return 0;
}