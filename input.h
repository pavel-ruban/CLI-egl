#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <boost/thread.hpp>
#include <linux/input.h>
#include <pcre.h>
#include <iomanip>
#include <locale>
#include <sstream>
#include <math.h>
#include <sys/select.h>
#include <sys/types.h>

char* parseInputHandler(int, char*);
void handleEvent(int);
typedef struct {
  int mouse;
  int kbd;
} inputFds;

inputFds initInput();

