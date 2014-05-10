#include <stdio.h>
#include <string>
#include <string.h>

int main(int argc, char* argv[]) {
  using namespace std;
  string s = to_string(3);
  printf("%s", s.c_str());

  return 0;
}
