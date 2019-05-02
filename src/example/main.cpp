#include "event_provider.h"

#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <chrono>

void RandomSleep() {
  struct timespec t, empty;
  t.tv_sec = 0;
  int ran = rand() % 300 + 100;
  t.tv_nsec = ran * 1000000L;
  nanosleep(&t, &empty);
}

void Sleep10ms() {
  struct timespec t, empty;
  t.tv_sec = 0;
  t.tv_nsec = 10 * 1000000L;
  nanosleep(&t, &empty);
}

int main() {
  srand(time(NULL));
  while (true) {
    MainFrame();
    std::chrono::high_resolution_clock::time_point start =
        std::chrono::high_resolution_clock::now();
    int startMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                      start.time_since_epoch())
                      .count();

    // fake payload
    RandomSleep();

    std::chrono::high_resolution_clock::time_point end =
        std::chrono::high_resolution_clock::now();
    int endMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                    end.time_since_epoch())
                    .count();

    STimeIntervalArg a;
    a.startTime = startMs;
    a.endTime = endMs;

    STracingInterval a2;
    a2.startTime = startMs;
    a2.endTime = endMs;
    a2.tid = 100;
    a2.name = "NameFoo";
    a2.category = "CategoryBar";
    a2.module = 42;

    ProfileEvent(a2);

    Sleep10ms();
  }

  return 0;
}
