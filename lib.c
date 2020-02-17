#include <windows.h>

__thread int value;

void
function(void) {
    value = GetCurrentThreadId();
}
