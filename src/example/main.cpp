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

void Foo2() {
  TRACE("", "");
  RandomSleep();
}

void Foo() {
  TRACE("", "");
  RandomSleep();
  Foo2();
}

void PhysThread() {
  while (true) {
    TRACE("", "");
    RandomSleep();
    Foo();
    RandomSleep();
  }
}

void RenderThread() {
  while (true) {
    TRACE("", "");
    Foo();
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
      TRACE("", "");
      //      TRACE_MAIN_FRAME();
      MainFrame();
      Foo();
      // fake payload
      RandomSleep();
    }

    Sleep10ms();
  }

  return 0;
}
