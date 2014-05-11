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

void m(abc*);
void m2(abc*);

void t1() {
  while (1) {
    printf("thread1: %i\n", z->bb->run);
  }
}

void t2() {
    m2(z);
  while (1) {
    printf("thread2: %i\n", z->bb->run);
  }
}

int main(int argc, char* argv[]) {
  using namespace std;
  z = (abc*) malloc(sizeof(abc));
  z->bb = (bbb*) malloc(sizeof(bbb));
  z->bb->run = 1;

  boost::thread graphicThread(t1);
  boost::thread eventThread(t2);

  graphicThread.join();
  eventThread.join();

  return 0;
}
