#include "common.hpp"
#include "context.hpp"
#include "gl.hpp"
#include "render.hpp"
#include "input.h"

Context context;

bool run = true, debug = true;

/**
 * Init graphic & start render process.
 */
int graphic_thread() {
  ESContext esContext;
  UserData userData;

  // Init free type.
  if(FT_Init_FreeType(&context.freetype.ft)) {
    fprintf(stderr, "Could not init freetype library\n");
    return 1;
  }
  if(FT_New_Face(context.freetype.ft, "UbuntuMono-R.ttf", 0, &context.freetype.face)) {
    fprintf(stderr, "Could not open font\n");
    return 1;
  } 

  context.freetype.g = context.freetype.face->glyph; 

  FT_Set_Pixel_Sizes(context.freetype.face, 0, 18);

  esInitContext(&esContext);
  esContext.userData = &userData;

  esCreateWindow(&esContext, "Hello Triangle", 1080, 1080, ES_WINDOW_RGB);

  if (!Init(&esContext))
    return 0;

  esRegisterDrawFunc(&esContext, Draw);

  esMainLoop(&esContext);
}

/**
 * Handle input handlers.
 */
void event_thread() {
  int retval, max_fd;
  fd_set input;
  inputFds inputFds = initInput(); 

  if (inputFds.mouse < 3 || inputFds.kbd < 3) {
    printf("error: there are no any active input drivers");
    exit(1);
  }

  max_fd = (inputFds.mouse > inputFds.kbd ? inputFds.mouse : inputFds.kbd) + 1;

  printf("mouse fd = %i\nkeyboard fd = %i", inputFds.mouse, inputFds.kbd);

  while (run) {
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
        handleEvent(inputFds.mouse);
      }
      else if (FD_ISSET(inputFds.kbd, &input)) {
        handleEvent(inputFds.kbd);
      }
    }
  }
}

/**
 * Programm start.
 */
int main(int argc, char *argv[]) {
  // Split program into two separate threads.
  boost::thread eventThread(event_thread);
  boost::thread graphicThread(graphic_thread);

  eventThread.join();
  graphicThread.join();

  return 0;
}
