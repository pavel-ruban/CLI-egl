/**
 * DRM mode setting.
 */

#include "context.hpp"

static int init_drm(Context* context);
static int init_gbm(Context* context);
static int init_gl(Context* context);

static void drm_fb_destroy_callback(struct gbm_bo *bo, void *data);
static struct drm_fb* drm_fb_get_from_bo(struct gbm_bo *bo, Context* context);
static void page_flip_handler(int fd, unsigned int frame, unsigned int sec, unsigned int usec, void *data);
static void modeset_cleanup(int fd, Context* context);
