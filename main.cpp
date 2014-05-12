#include "common.hpp"
#include "context.hpp"
#include "gl.hpp"
#include "render.hpp"
#include "input.h"
#include <boost/thread.hpp>

#ifdef _egl_use_drm
  #include "drm.hpp"
#elif defined _egl_use_x11
#endif

/**
 * Init graphic & start render process.
 */
void graphic_thread(CONTEXT_TYPE context) {
  #ifdef _egl_use_drm
    ret = init_drm(context);

    if (ret) {
      printf("failed to initialize DRM\n");
      return ret;
    }

    ret = init_gbm(context);
    if (ret) {
      printf("failed to initialize GBM\n");
      return ret;
    }

    ret = init_gl(context);
    if (ret) {
      printf("failed to initialize EGL\n");
      return ret;
    }

    GL* gl = &context->gl;
    Gbm* gbm = &context->gbm;
    Drm* drm = &context->drm;

    /* clear the color buffer */
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    eglSwapBuffers(gl->display, gl->surface);
    bo = gbm_surface_lock_front_buffer(gbm->surface);
    fb = drm_fb_get_from_bo(bo);

    /* set mode: */
    ret = drmModeSetCrtc(drm->fd, drm->crtc_id, fb->fb_id, 0, 0,
        &drm->connector_id, 1, drm->mode);

    if (ret) {
      printf("failed to set mode: %s\n", strerror(errno));
      return ret;
    }

	  fd_set fds;
    drmEventContext e;
    e.version = DRM_EVENT_CONTEXT_VERSION;
    e.page_flip_handler = page_flip_handler;
    struct gbm_bo *bo;
    struct drm_fb *fb;

    while (context->flags.run) {
      struct gbm_bo *next_bo;
      int waiting_for_flip = 1;

      Draw(context);

      eglSwapBuffers(gl->display, gl->surface);
      next_bo = gbm_surface_lock_front_buffer(gbm->surface);
      fb = drm_fb_get_from_bo(next_bo, context);

      /*
       * Here you could also update drm plane layers if you want
       * hw composition
       */
      ret = drmModePageFlip(drm->fd, drm->crtc_id, fb->fb_id,
          DRM_MODE_PAGE_FLIP_EVENT, &waiting_for_flip);
      if (ret) {
        printf("failed to queue page flip: %s\n", strerror(errno));
        return -1;
      }

      while (waiting_for_flip && context->flags.run) {
        FD_ZERO(&fds);
        FD_SET(drm->fd, &fds);

        ret = select(drm->fd + 1, &fds, NULL, NULL, NULL);

        if (ret < 0) {
          printf("select err: %s\n", strerror(errno));
          return ret;
        } else if (FD_ISSET(drm->fd, &fds)) {
          drmHandleEvent(drm->fd, &e);
        }
      }

      /* release last buffer to render on again: */
      gbm_surface_release_buffer(gbm->surface, bo);
      bo = next_bo;
    }

    // Release drm resources.
    modeset_cleanup(drm->fd);
  #elif defined _egl_use_x11
    esCreateWindow(context, "OpenGLEv2", 1080, 1080, ES_WINDOW_RGB);

    if (!Init(context))
      return;

    esRegisterDrawFunc(context, Draw);

    esMainLoop(context);
  #endif
}

/**
 * Handle input handlers.
 */
void event_thread(CONTEXT_TYPE context) {
  int retval, max_fd;
  fd_set input;
  inputFds inputFds = initInput(); 

  if (inputFds.mouse < 3 || inputFds.kbd < 3) {
    printf("error: there are no any active input drivers");
    exit(1);
  }

  max_fd = (inputFds.mouse > inputFds.kbd ? inputFds.mouse : inputFds.kbd) + 1;

  printf("mouse fd = %i\nkeyboard fd = %i", inputFds.mouse, inputFds.kbd);

  // Check context flag to break cycle.
  while (context->userData.run) {
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
        handleEvent(inputFds.mouse, context);
      }
      else if (FD_ISSET(inputFds.kbd, &input)) {
        handleEvent(inputFds.kbd, context);
      }
    }
  }
}

/**
 * Program start.
 */
int main(int argc, char *argv[]) {

  #ifdef _egl_use_drm
    Context* context;
    context = (Context*) malloc(sizeof(Context));
    memset(context, 0, sizeof(Context) );
  #elif defined _egl_use_x11
    // Init context to avoid useless global variables.
    ESContext* context;
    context = (ESContext*) malloc(sizeof(ESContext));
    esInitContext(context);
  #endif

  context->flags.run = true;
  context->flags.debug = true;

  // Split program into two separate threads.
  boost::thread graphicThread(boost::bind(graphic_thread, boost::ref(context)));
  boost::thread eventThread(boost::bind(event_thread, boost::ref(context)));

  graphicThread.join();
  eventThread.join();

  return 0;
}
