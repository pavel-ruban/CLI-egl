#include "common.hpp"
#include "gl.hpp"
#include "render.hpp"
#include "input.h"
#include <boost/thread.hpp>

/**
 * Init graphic & start render process.
 */
int graphic_thread(ESContext* esContext) {
  esCreateWindow(esContext, "Hello Triangle", 1080, 1080, ES_WINDOW_RGB);

  if (!Init(esContext))
    return 0;

  esRegisterDrawFunc(esContext, Draw);

  esMainLoop(esContext);
}

/**
 * Handle input handlers.
 */
void event_thread(ESContext* esContext) {
  int retval, max_fd;
  fd_set input;
  inputFds inputFds = initInput(); 

  if (inputFds.mouse < 3 || inputFds.kbd < 3) {
    printf("error: there are no any active input drivers");
    exit(1);
  }

  max_fd = (inputFds.mouse > inputFds.kbd ? inputFds.mouse : inputFds.kbd) + 1;

  printf("mouse fd = %i\nkeyboard fd = %i", inputFds.mouse, inputFds.kbd);

  while (esContext->userData.run) {
    /* Initialize the input set */
    FD_ZERO(&input);
    FD_SET(inputFds.mouse, &input);
    FD_SET(inputFds.kbd, &input);

    /* Do the select */
    retval = select(max_fd, &input, NULL, NULL, NULL);

    /* See if there was an error */
    if (retval < 0)
      perror("select failed");
    else if (retval == 0)
      puts("TIMEOUT");
    else {
      /* We have input */
      if (FD_ISSET(inputFds.mouse, &input)) {
        handleEvent(inputFds.mouse, esContext);
      }
      else if (FD_ISSET(inputFds.kbd, &input)) {
        handleEvent(inputFds.kbd, esContext);
      }
    }
  }
}

/**
 * Programm start.
 */
int main(int argc, char *argv[]) {
  ESContext* esContext;
  esContext = (ESContext*) malloc(sizeof(ESContext));

  esInitContext(esContext);

  esContext->userData.run = true;
  esContext->userData.debug = true;

  // Split program into two separate threads.
  boost::thread graphicThread(boost::bind(graphic_thread, boost::ref(esContext)));
  boost::thread eventThread(boost::bind(event_thread, boost::ref(esContext)));

  graphicThread.join();
  eventThread.join();

  return 0;
}
