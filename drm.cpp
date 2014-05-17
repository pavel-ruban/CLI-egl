/**
 * DRM mode setting.
 */

#include "context.hpp"
#include "gl.hpp"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

int init_drm(Context* context) {
  Drm* drm = &context->drm;
  static const char *modules[] = {
    "i915", "radeon", "nouveau", "nvidia", "vmwgfx", "omapdrm", "exynos", "msm"
	};

	drmModeRes *resources;
	drmModeConnector *connector = NULL;
	drmModeEncoder *encoder = NULL;
	int i, area;

	for (i = 0; i < ARRAY_SIZE(modules); i++) {
		printf("trying to load module %s...", modules[i]);
		drm->fd = drmOpen(modules[i], NULL);
		if (drm->fd < 0) {
			printf("failed.\n");
		} else {
			printf("success.\n");
			break;
		}
	}

	if (drm->fd < 0) {
		printf("could not open drm device\n");
		return -1;
	}

	resources = drmModeGetResources(drm->fd);
	if (!resources) {
		printf("drmModeGetResources failed: %s\n", strerror(errno));
		return -1;
	}

	/* find a connected connector: */
	for (i = 0; i < resources->count_connectors; i++) {
		connector = drmModeGetConnector(drm->fd, resources->connectors[i]);
		if (connector->connection == DRM_MODE_CONNECTED) {
			/* it's connected, let's use this! */
			break;
		}
		drmModeFreeConnector(connector);
		connector = NULL;
	}

	if (!connector) {
		/* we could be fancy and listen for hotplug events and wait for
		 * a connector..
		 */
		printf("no connected connector!\n");
		return -1;
	}

	/* find highest resolution mode: */
	for (i = 0, area = 0; i < connector->count_modes; i++) {
		drmModeModeInfo *current_mode = &connector->modes[i];
		int current_area = current_mode->hdisplay * current_mode->vdisplay;
		if (current_area > area) {
			drm->mode = current_mode;
			area = current_area;
		}
	}

	if (!drm->mode) {
		printf("could not find mode!\n");
		return -1;
	}

	/* find encoder: */
	for (i = 0; i < resources->count_encoders; i++) {
		encoder = drmModeGetEncoder(drm->fd, resources->encoders[i]);
		if (encoder->encoder_id == connector->encoder_id)
			break;
		drmModeFreeEncoder(encoder);
		encoder = NULL;
	}

	if (!encoder) {
		printf("no encoder!\n");
		return -1;
	}

	drm->crtc_id = encoder->crtc_id;
	drm->connector_id = connector->connector_id;
  drm->saved_crtc = drmModeGetCrtc(drm->fd, drm->crtc_id);

	return 0;
}

int init_gbm(Context* context) {
  Gbm* gbm = &context->gbm;
  Drm* drm = &context->drm;

  gbm->dev = gbm_create_device(drm->fd);

	gbm->surface = gbm_surface_create(gbm->dev,
			drm->mode->hdisplay, drm->mode->vdisplay,
			GBM_FORMAT_XRGB8888,
			GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);

	if (!gbm->surface) {
		printf("failed to create gbm surface\n");
		return -1;
	}

	return 0;
}

int init_gl(Context* context) {
  GL* gl = &context->gl;
  Gbm* gbm = &context->gbm;
  Drm* drm = &context->drm;

	EGLint major, minor, n;
	GLint ret;

	static const EGLint context_attribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	static const EGLint config_attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RED_SIZE, 1,
		EGL_GREEN_SIZE, 1,
		EGL_BLUE_SIZE, 1,
		EGL_ALPHA_SIZE, 0,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};

	gl->display = eglGetDisplay((EGLNativeDisplayType) gbm->dev);

	if (!eglInitialize(gl->display, &major, &minor)) {
		printf("failed to initialize\n");
		return -1;
	}

	printf("Using display %p with EGL version %d.%d\n",
			gl->display, major, minor);

	printf("EGL Version \"%s\"\n", eglQueryString(gl->display, EGL_VERSION));
	printf("EGL Vendor \"%s\"\n", eglQueryString(gl->display, EGL_VENDOR));
	printf("EGL Extensions \"%s\"\n", eglQueryString(gl->display, EGL_EXTENSIONS));

	if (!eglBindAPI(EGL_OPENGL_ES_API)) {
		printf("failed to bind api EGL_OPENGL_ES_API\n");
		return -1;
	}

	if (!eglChooseConfig(gl->display, config_attribs, &gl->config, 1, &n) || n != 1) {
		printf("failed to choose config: %d\n", n);
		return -1;
	}

	gl->context = eglCreateContext(gl->display, gl->config,
			EGL_NO_CONTEXT, context_attribs);
	if (gl->context == NULL) {
		printf("failed to create context\n");
		return -1;
	}

	gl->surface = eglCreateWindowSurface(gl->display, gl->config, (EGLNativeWindowType) gbm->surface, NULL);
	if (gl->surface == EGL_NO_SURFACE) {
		printf("failed to create egl surface\n");
		return -1;
	}

	/* connect the context to the surface */
	eglMakeCurrent(gl->display, gl->surface, gl->surface, gl->context);

	glViewport(0, 0, drm->mode->hdisplay, drm->mode->vdisplay);

  Init(context);

	return 0;
}

void drm_fb_destroy_callback(struct gbm_bo *bo, void *data) {
  CONTEXT_TYPE *context = (CONTEXT_TYPE*) data;
	struct drm_fb *fb = context->drm.fb;
	struct gbm_device *gbm = gbm_bo_get_device(bo);
  Drm *drm = &context->drm;

	if (fb->fb_id)
		drmModeRmFB(drm->fd, fb->fb_id);

	free(fb);
}

struct drm_fb* drm_fb_get_from_bo(struct gbm_bo *bo, CONTEXT_TYPE *context) {
	struct drm_fb *fb = (drm_fb*) gbm_bo_get_user_data(bo);
	uint32_t width, height, stride, handle;
  Drm* drm = &context->drm;
	int ret;

	if (fb)
		return fb;

	fb = (drm_fb*) calloc(1, sizeof *fb);
	fb->bo = bo;

	width = gbm_bo_get_width(bo);
	height = gbm_bo_get_height(bo);
	stride = gbm_bo_get_stride(bo);
	handle = gbm_bo_get_handle(bo).u32;

	ret = drmModeAddFB(drm->fd, width, height, 24, 32, stride, handle, &fb->fb_id);
	if (ret) {
		printf("failed to create fb: %s\n", strerror(errno));
		free(fb);
		return NULL;
	}

  drm->fb = fb;
	gbm_bo_set_user_data(bo, fb, drm_fb_destroy_callback);

	return fb;
}

void page_flip_handler(int fd, unsigned int frame,
		  unsigned int sec, unsigned int usec, void *data) {
	int *waiting_for_flip = (int*) data;
	*waiting_for_flip = 0;
}

void modeset_cleanup(int fd, Context* context) {
  Drm* drm = &context->drm;
  /* restore saved CRTC configuration */
  drmModeSetCrtc(fd,
      drm->saved_crtc->crtc_id,
      drm->saved_crtc->buffer_id,
      drm->saved_crtc->x,
      drm->saved_crtc->y,
      &drm->connector_id,
      1,
      &drm->saved_crtc->mode);
  drmModeFreeCrtc(drm->saved_crtc);
}
