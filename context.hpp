/**
 * This header contains includes used in main threads sources.
 */

#include "esUtil.h"
#include <ft2build.h>
#include FT_FREETYPE_H

typedef struct {
  FT_Library ft;
  FT_Face face;
  FT_GlyphSlot g;
} Freetype;

typedef struct {
  GLint uniform_istext;
  GLint uniform_color;
} Uniforms;

typedef struct {
  // Handle to a program object
  GLuint programObject;
  GLuint vbo[2];
  Freetype freetype;
  Uniforms uniforms;
  bool run;
  bool debug;
} UserData;
