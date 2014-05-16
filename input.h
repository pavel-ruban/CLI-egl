#include "context.hpp"
char* parseInputHandler(int, char*);

void handleEvent(int, CONTEXT_TYPE*);

typedef struct {
  int mouse;
  int kbd;
} inputFds;

inputFds initInput();
