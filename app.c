#include <stdio.h>

#include <windows.h>

#define NTHREAD 2

extern __thread int value;
void function(void);

DWORD
thread_main(LPVOID param) {
    DWORD tid = GetCurrentThreadId();
    printf("%lu: param = %d\n", tid, (int)param);
    fflush(stdout);
    value = (int)param;
    printf("%lu: value (old) = %d\n", tid, value);
    fflush(stdout);
    function();
    printf("%lu: value (new) = %d\n", tid, value);
    fflush(stdout);
    return 0;
}

int
main(int argc, char** argv) {
    HANDLE ts[NTHREAD];
    int i;

    for (i = 0; i < NTHREAD; i++) {
        ts[i] = CreateThread(NULL, 0, thread_main, (LPVOID)i, 0, NULL);
    }
    WaitForMultipleObjects(NTHREAD, ts, TRUE, INFINITE);

    return value;
}
