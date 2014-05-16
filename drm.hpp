/**
 * DRM mode setting.
 */

#include "context.hpp"

int init_drm(CONTEXT_TYPE*);
int init_gbm(CONTEXT_TYPE*);
int init_gl(CONTEXT_TYPE*);

void drm_fb_destroy_callback(struct gbm_bo*, void*);
struct drm_fb* drm_fb_get_from_bo(struct gbm_bo*, CONTEXT_TYPE*);
void page_flip_handler(int fd, unsigned int frame, unsigned int sec, unsigned int usec, void *data);
void modeset_cleanup(int fd, CONTEXT_TYPE*);
