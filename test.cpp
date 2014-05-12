#include <stdio.h>
#include <string>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/egl.h>
#include <vector>
#include <array>
#include <boost/thread.hpp>

using namespace std;


typedef struct {
  int run;
} bbb;

typedef struct {
  bbb* bb;
} abc;

abc* z;

void t1() {
  while (1) {
    printf("thread1: %i\n", z->bb->run);
  }
}

void t2() {
  while (1) {
    printf("thread2: %i\n", z->bb->run);
  }
}

int main(int argc, char* argv[]) {
  #define ABC
  #ifndef ABC
    using namespace std;
    z = (abc*) malloc(sizeof(abc));
    z->bb = (bbb*) malloc(sizeof(bbb));
    z->bb->run = 1;

    boost::thread graphicThread(t1);
    boost::thread eventThread(t2);

    graphicThread.join();
    eventThread.join();
  #else
    struct ac {
      int x;
      int b;
    };

    ac n;
    n.x = 3;
    n.b = 6;
    ac v;

    v = n;

    printf("hey hey!! %i, %i\n", v.x, v.b);
  #endif

  return 0;
}
