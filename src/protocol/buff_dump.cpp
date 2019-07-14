#include <stdio.h>
#include <iostream>

void Dump(const char* buffer, size_t len) {
  std::cout << std::noskipws;
  for (int i = 0; i < len; ++i) {
    char c = buffer[i];

    char tmpStr[10];
    snprintf(tmpStr, 4, "%02x ", c);
    std::cout << tmpStr;

    if (i % 20 == 0 && i > 0) std::cout << std::endl;
  }
  std::cout << std::endl << std::skipws;
}
