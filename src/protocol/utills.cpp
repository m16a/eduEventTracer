#include "utills.h"

#include <sys/syscall.h>
#include <unistd.h>
#define gettid() syscall(SYS_gettid)

int GetTid() { return gettid(); }
