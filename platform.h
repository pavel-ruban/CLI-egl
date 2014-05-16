#ifndef _egl_use_drm
  #define _egl_use_drm
#endif

//#define _egl_use_x11

#ifndef _egl_initialized
  #define _egl_initialized

  #include "esUtil.h"

  #ifdef _egl_use_drm
    #define CONTEXT_TYPE Context
    #include <xf86drm.h>
    #include <xf86drmMode.h>
    #include <gbm.h>
    #include <errno.h>

  #elif defined _egl_use_x11
    #define CONTEXT_TYPE ESContext
  #endif
#endif
