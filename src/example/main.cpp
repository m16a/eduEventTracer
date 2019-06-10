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
    TRACE("", "");
    RandomSleep();
  }
}

void RenderThread() {
  while (true) {
    TRACE("", "");
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
    {
      TRACE_MAIN_FRAME();
      MainFrame();
      // fake payload
      RandomSleep();
    }

    Sleep10ms();
  }

  return 0;
}
