#include "context.hpp"

char* parseInputHandler(int, char*);

void handleEvent(int, ESContext*);

typedef struct {
  int mouse;
  int kbd;
} inputFds;

inputFds initInput();
