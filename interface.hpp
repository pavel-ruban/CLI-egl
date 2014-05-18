#include "context.hpp"

// Default handlers.
void default_handler(CONTEXT_TYPE*);
void increment(CONTEXT_TYPE*);
void decrement(CONTEXT_TYPE*);
void incrementAngle(CONTEXT_TYPE*);
void decrementAngle(CONTEXT_TYPE*);
void toggleAngle(CONTEXT_TYPE*);

class button {
  private:
    void (*handler)(CONTEXT_TYPE*);
    void submit(CONTEXT_TYPE*);

  public:
    button();
    button(float, float, void (*handler) (CONTEXT_TYPE*) = default_handler);
    button(float, float, float, float, void (*handler) (CONTEXT_TYPE*) = default_handler);
    void trigger(const char*, CONTEXT_TYPE*);
    bool hover, click;
    GLfloat height, width, x, y;
};
