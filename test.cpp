#include <stdio.h>
#include <string>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/egl.h>
#include <vector>
#include <array>

using namespace std;

typedef struct {
  int a[5];
} abc;

void m(abc* x) {
  int *b = (*x).a;
  
  //b = (*x).a;

  printf("array: %i", b[2]);
}

int main(int argc, char* argv[]) {
  using namespace std;

    
  m(&x);
  // printf("\n%i ", second.weight);

  return 0;
}
