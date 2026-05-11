#pragma once
#include <windows.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    UI_COLOR_DEFAULT,
    UI_COLOR_RED,
    UI_COLOR_GREEN,
    UI_COLOR_YELLOW,
    UI_COLOR_BLUE,
    UI_COLOR_MAGENTA,
    UI_COLOR_CYAN,
    UI_COLOR_WHITE
} UI_Color;

void UI_Init();
void UI_Clear();
void UI_SetColor(UI_Color color);
void UI_MoveCursor(int row, int col);
void UI_DrawLine(int width);
void UI_DrawMenu();
void DrawStatus(const char *status, UI_Color color);
void UI_Log(const char* fmt, ...);
const char* UI_SelectReplayFile(const char* dir_path);

#ifdef __cplusplus
}
#endif