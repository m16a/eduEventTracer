#include "event_provider.h"

#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <chrono>
#include <thread>

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

void WorkerThread() {}

void PhysThread() {
  while (true) {
    RandomSleep();
  }
}

void RenderThread() {
  while (true) {
    RandomSleep();
  }
}

void StartThreads() {
  std::thread rend(RenderThread);
  rend.detach();

  std::thread phys(PhysThread);
  phys.detach();
}

int main() {
  srand(time(NULL));
  StartThreads();
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

    STracingInterval a;
    a.startTime = startMs;
    a.endTime = endMs;
    a.tid = 100;
    a.name = "NameFoo";
    a.category = "CategoryBar";
    a.module = 42;

    ProfileEvent(a);

    Sleep10ms();
  }

  return 0;
}
