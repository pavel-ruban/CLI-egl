/**
 * This header contains includes used in main threads sources.
 */

#ifndef _egl_context_initilized
  #define _egl_context_initilized
  #include "platform.h"

  #include <ft2build.h>
  #include FT_FREETYPE_H

  struct Freetype {
    FT_Library ft;
    FT_Face face;
    FT_GlyphSlot g;
  };

  struct Uniforms {
    GLint uniform_istext;
    GLint uniform_color;
  };

  struct Flags {
    bool run;
    bool debug;
  };

  struct Vars {
    GLfloat mAngle;
    GLfloat mAngleSin;
    GLfloat mAngleCos;
    GLfloat mouseX;
    GLfloat mouseY;
    bool aid;
    bool pressed;
  };

  #ifdef _egl_use_drm

  struct GL {
    EGLDisplay display;
    EGLConfig config;
    EGLContext context;
    EGLSurface surface;
    GLuint program;
    GLint modelviewmatrix, modelviewprojectionmatrix, normalmatrix;
    GLuint vbo[2];
    GLuint positionsoffset, colorsoffset, normalsoffset;
  };

  struct Gbm {
    struct gbm_device *dev;
    struct gbm_surface *surface;
  };

  struct Drm {
    int fd;
    drmModeModeInfo *mode;
    uint32_t crtc_id;
    uint32_t connector_id;
    drmModeCrtc *saved_crtc;
    struct drm_fb *fb;
  };

  struct drm_fb {
    struct gbm_bo *bo;
    uint32_t fb_id;
  };

  struct Context {
    GL gl;
    Drm drm;
    Gbm gbm;
    Flags flags;
    Freetype freetype;
    Uniforms uniforms;
    Vars vars;
  };

  #elif defined _egl_use_x11

  #include "esUtil.h"

  typedef struct {
    // Handle to a program object
    GLuint programObject;
    GLuint vbo[2];
    Freetype freetype;
    Uniforms uniforms;
    bool run;
    bool debug;
  } UserData;

  #endif
#endif
