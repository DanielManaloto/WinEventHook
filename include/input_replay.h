#pragma once
#include <windows.h>
#ifdef __cplusplus
extern "C" {
#endif

void UI_LogKeyboardEvent(int index, const INPUT* in);
void UI_LogMouseEvent(int index, const INPUT* in);
void DumpEventFile(const char *filename);
void Replay(const char *filename);

#ifdef __cplusplus
}
#endif