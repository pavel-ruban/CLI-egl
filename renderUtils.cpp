#include "esUtil.h"
#include "context.hpp"

void render_text(CONTEXT_TYPE *context, const char *text, float x, float y, float sx, float sy) {

  Freetype *freetype = &context->freetype;
  const char *p;
 
  for(p = text; *p; p++) {
    if(FT_Load_Char(freetype->face, *p, FT_LOAD_RENDER))
        continue;
 
    FT_GlyphSlot g;
    g = freetype->g;

    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_ALPHA,
      g->bitmap.width,
      g->bitmap.rows,
      0,
      GL_ALPHA,
      GL_UNSIGNED_BYTE,
      g->bitmap.buffer
    );
 
    float x2 = x + g->bitmap_left * sx;
    float y2 = -y - g->bitmap_top * sy;
    float w = g->bitmap.width * sx;
    float h = g->bitmap.rows * sy;
 
    GLfloat box[4][4] = {
        {x2,     -y2    , 0, 0},
        {x2 + w, -y2    , 1, 0},
        {x2,     -y2 - h, 0, 1},
        {x2 + w, -y2 - h, 1, 1},
    };
 
    glBindBuffer(GL_ARRAY_BUFFER, context->gl.vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
 
    x += (g->advance.x >> 6) * sx;
    y += (g->advance.y >> 6) * sy;
  }
}

void multipleMatrices4x4(float* vVertices, float* mRotate) {
  // Store unchanged values to temp storage.
  float t[16];
  memcpy(t, vVertices, 16 * sizeof(float));
  int offset = 0, position = 0;
  int mPosition = 0;

  for (int i = 0; i < 4; i++) {
    mPosition = position = 0;
    for (int i = 0; i < 4; i++) {
      vVertices[offset * 4 + position] = t[offset * 4] * mRotate[mPosition] + t[offset * 4 + 1] * mRotate[mPosition + 4] + t[offset * 4 + 2] * mRotate[mPosition + 8] + t[offset * 4 + 3] * mRotate[mPosition + 12];
      mPosition++;
      position++;
    }
    offset++;
  }
}
