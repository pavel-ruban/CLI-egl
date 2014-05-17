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
    #include <linux/input.h>
    #include <time.h>
    #include <unistd.h>
    #include <sys/select.h>
    #include <sys/mman.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <fstream>
    #include <vector>
    #include <array>
    #include <string>
    #include <math.h>

  #elif defined _egl_use_x11
    #define CONTEXT_TYPE ESContext
  #endif
#endif
