#include "context.hpp"
#include "interface.hpp"

using namespace std;

button::button() {
  x = y = 0.0;
  width = 0.135;
  height = 0.05;
  hover = click = false;
  handler = default_handler;
}

button::button(float x, float y, void (*handler) (CONTEXT_TYPE*)) {
  button::x = x;
  button::y = y;
  width = 0.135;
  height = 0.05;
  button::handler = handler;
  hover = click = false;
}

button::button(float x, float y, float width, float height, void (*handler) (CONTEXT_TYPE*)) {
  button::x = x;
  button::y = y;
  button::width = width;
  button::height = height;
  hover = click = false;
  button::handler = handler;
}

void button::submit(CONTEXT_TYPE *context) {
  handler(context);  
}

void button::trigger(const char *type, CONTEXT_TYPE *context) {
  string subject(type, 0, 30);
  string submit("submit");
  int code = subject.compare(submit);

  switch (code) {
    case 0:
      button::submit(context);
      break;
    
  }
}

// Custom submit handlers.
void default_handler(CONTEXT_TYPE *context) {
  context->vars.counter++; 
}

void increment(CONTEXT_TYPE *context) {
  context->vars.counter++; 
}

void decrement(CONTEXT_TYPE *context) {
  context->vars.counter--; 
}

void incrementAngle(CONTEXT_TYPE *context) {
  context->vars.mAngle++;
}

void decrementAngle(CONTEXT_TYPE *context) {
  context->vars.mAngle--;
}

void toggleAngle(CONTEXT_TYPE *context) {
  context->vars.mAngleAuto ^= 1;
}

