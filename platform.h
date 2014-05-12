#define _egl_use_drm
//#define _egl_use_x11

#ifdef _egl_use_drm
  #define CONTEXT_TYPE Context*
#elif defined _egl_use_x11
  #define CONTEXT_TYPE ESContext*
#endif
