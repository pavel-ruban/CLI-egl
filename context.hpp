/**
 * This header contains includes used in main threads sources.
 */

#include "esUtil.h"
#include <ft2build.h>
#include FT_FREETYPE_H

struct freetype {
  FT_Library ft;
  FT_Face face;
  FT_GlyphSlot g;
};

struct Context {
  freetype freetype;
};
