#pragma once
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IR_MAX_EVENTS 10000

typedef struct {
    DWORD type;     
    DWORD event;
    DWORD time;
    DWORD delta;

    DWORD vkCode;
    DWORD scanCode;
    DWORD flags;

    POINT pt;
    DWORD mouseData;
} IR_Event;

typedef struct {
    IR_Event events[IR_MAX_EVENTS];
    int count;

    int recording;

    ULONGLONG startTime;
    ULONGLONG lastEventTime;

    HHOOK keyHook;
    HHOOK mouseHook;

} InputRecorder;

typedef struct {
    INPUT input;
    DWORD delta;
} IR_TimedInput;

int  IR_Init(InputRecorder *rec);
void IR_Shutdown(InputRecorder *rec);

void IR_Start(InputRecorder *rec);
void IR_Stop(InputRecorder *rec);

int IR_SaveToFile(InputRecorder *rec, const char *filename);

#ifdef __cplusplus
}
#endif