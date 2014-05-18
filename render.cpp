#include "esUtil.h"
#include "context.hpp"
#include "renderUtils.hpp"
#include "interface.hpp"

using namespace std;

/*
 * Draw a triangle using the shader pair created in Init()
 */
void Draw(CONTEXT_TYPE *context) {
  Drm drm = context->drm;

  // Set the viewport
  glViewport(0, 0, drm.mode->hdisplay, drm.mode->vdisplay);

  // Clear the color buffer
  glClear(GL_COLOR_BUFFER_BIT);


  // Define skeleton.
 // GLfloat skeleton[3][2] = {
 //   {45.0 + xAngle, 0.5 + xRadius},
 //   {0.0 + yAngle, 0.3 + yRadius},
 //   {90.0 + zAngle, 0.2 + zRadius},
 // };

 // int i = 0;

 // GLfloat x = 0.0, y = 0.0, z = 1.0;

  // Let's calculate matrices & vertex coordinates.
//  GLfloat vVertices[] = {
//    0.0f, 0.0f, 1.0f, 1.0f, 
//    0.0f, 0.0f, 0.0f, 1.0f,
//    0.0f, 0.0f, 0.0f, 1.0f,
//  };
//
//  #define RADIUS skeleton[i][1]
//  #define ANGLE skeleton[i][0]
  #define PI 3.14159265
//
//
//  while (i < 3) {
//    // If length equals to zero just set point's coordinates as 0.0.
//    if (RADIUS <= 0.0) {
//      x = 0.0;
//      y = 0.0;
//    }
//    // If angle equals to zero just calculate x and set y as 0.0.
//    else if (ANGLE <= 0.0)  {
//      x = RADIUS;
//      y = 0.0;
//    }
//    // Translate spheric coordinates into decart.
//    else {
//      x = RADIUS * cos(ANGLE * PI / 180);
//      y = RADIUS * sin(ANGLE * PI / 180);
//    }
//
//    vVertices[i * 3] = x;
//    vVertices[i * 3 + 1] = y;
//    vVertices[i * 3 + 2] = z;
//
//    i++;
//  } 

  float mAngle = context->vars.mAngle;
  static float tempAngle = 0.0;
  static float mAngleCos = 0.0;
  static float mAngleSin = 0.0;
  static float scale = 1080.00 / 1920.00 ;//drm.mode->vdisplay / drm.mode->hdisplay;
  
  if (mAngle > 360.00) {
    mAngle = context->vars.mAngle = 0.0;
  }

  if (tempAngle == 0.0 || tempAngle != mAngle) {
    mAngleSin = sin(mAngle * PI / 180);
    mAngleCos = cos(mAngle * PI / 180);
    tempAngle = mAngle;
  }

  static float scale2 = 1920.00 / 1080.00;//drm.mode->vdisplay / drm.mode->hdisplay;

  GLfloat mRotate[] {
    mAngleCos, -mAngleSin,  0.0, 0.0,
   mAngleSin,  mAngleCos,  0.0, 0.0,
    0.0,        0.0,        1.0, 0.0,
    0.0,        0.0,        0.0, 1.0,
  };
  
  GLfloat vVertices[] = {
    -0.25, 0.25, 0.0, 1.0,
    0.25, 0.25, 0.0, 1.0,
    -0.25, -0.25, 0.0, 1.0,
    0.25, -0.25, 0.0, 1.0,
  };

  multipleMatrices4x4(vVertices, mRotate);

  vVertices[0] *= scale;
  vVertices[4] *= scale;
  vVertices[8] *= scale;
  vVertices[12] *= scale;

  GLfloat texCoords[4][2] = {
    {0, 0},
    {1, 0},
    {0, 1},
    {1, 1},
  };

  if (context->vars.mAngleAuto) {
    context->vars.mAngle += 1.0;
  }

  glUniform4f(context->uniforms.uniform_color, 0.0, 0.0, 1.0, 1.0);

  glBindBuffer(GL_ARRAY_BUFFER, context->gl.vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof texCoords, texCoords, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, context->gl.vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof vVertices, vVertices, GL_DYNAMIC_DRAW);

  glUniform1i(context->uniforms.uniform_istext, 0);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  
  glUniform4f(context->uniforms.uniform_color, 1.0, 1.0, 1.0, 1.0);
  Vars vars = context->vars;

  array<GLfloat, 16> vertices;


  float stride = 0.0;
  float btn_width = 0.15;
  float btn_height = 0.05;
  float gasp = 0.015;
  static float offsetY = 1.0, offsetX = 1.0;
  static int counter = 0;

  for (int i = 0; i < 13; i++) {
    void (*handler) (CONTEXT_TYPE*);
    
    switch (i) {
      case 1:
        handler = decrement;
        break;

      case 2:
        handler = incrementAngle;
        break;

      case 3:
        handler = decrementAngle;
        break;

      case 4:
        handler = toggleAngle;
        break;

      default:
        handler = increment;
        break;
      
    }

    button btn(offsetX - gasp - btn_width, offsetY - gasp - btn_height - stride, handler);

    stride += gasp + btn.height;

    vertices = {
      btn.x, btn.y, 0.0, 1.0,
      btn.x, btn.y + btn.height, 0.0, 1.0,
      btn.x + btn.width, btn.y, 0.0, 1.0,
      btn.x + btn.width, btn.y + btn.height, 0.0, 1.0,
    };

    if (vars.mouseX >= btn.x && vars.mouseX <= btn.x + btn.width && vars.mouseY >= btn.y && vars.mouseY <= btn.y + btn.height)
      btn.hover = true;

    if (btn.hover) {
      if (vars.pressed) {
          btn.trigger("submit", context);

        glUniform4f(context->uniforms.uniform_color, 0.0, 1.0, 0.0, 1.0);
      }
      else {
        glUniform4f(context->uniforms.uniform_color, 0.4, 0.4, 0.4, 1.0);
      }
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, (void*) vertices.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    if (btn.hover)
      glUniform4f(context->uniforms.uniform_color, 1.0, 1.0, 1.0, 1.0);
  }

    vertices = {
      vars.mouseX, vars.mouseY, 0.0, 1.0,
      vars.mouseX + 0.021, vars.mouseY - 0.03, 0.0, 1.0,
      vars.mouseX + 0.007, vars.mouseY - 0.05, 0.0, 1.0,
      0.0, 1.0, 0.0, 1.0,
    };

  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, (void*) vertices.data(), GL_DYNAMIC_DRAW);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

  static int ii = 0;
  float sx = 2.0 / drm.mode->hdisplay;
  float sy = 2.0 / drm.mode->vdisplay;
  static char buff[1000], message[100];
  glUniform1i(context->uniforms.uniform_istext, 1);
  if (context->flags.debug) {

    FT_Set_Pixel_Sizes(context->freetype.face, 0, 18);
    glUniform1i(context->uniforms.uniform_istext, 1);

    float rx = 0.0, ry = 0.0;
    int offset = 0;

    stride = -0.1;

    for (int i = 0; i < 4; i++) {
      stride += 0.04;

      switch (i) {

        case 0:
          sprintf(buff, "rendered frames %i", ii);
          break;

        case 1:
          stride += 0.04;
          sprintf(buff, "angles:");
          render_text(context, buff, -1 + 8 * sx, 1 - stride - 50 * sy,    sx, sy);
          stride += 0.04;
          sprintf(buff, "---------------------------");
          render_text(context, buff, -1 + 8 * sx, 1 - stride - 50 * sy,    sx, sy);
          stride += 0.04;
          sprintf(buff, "angle a %f", vars.mAngle);
          break;

        case 2:
          sprintf(buff, "sin(a) %f", vars.mAngleSin);
          break;

        case 3:
          sprintf(buff, "cos(a) %f", vars.mAngleCos);
          break;
      }

      render_text(context, buff, -1 + 8 * sx, 1 - stride - 50 * sy,    sx, sy);
    }

    stride += 0.08;
    sprintf(buff, "Vertices:");
    render_text(context, buff, -1 + 8 * sx, 1 - stride - 50 * sy,    sx, sy);
    stride += 0.04;
    sprintf(buff, "---------------------------");
    render_text(context, buff, -1 + 8 * sx, 1 - stride - 50 * sy,    sx, sy);

    for (int i = 0; i < 4; i++) {
      stride += 0.04;
      rx = vVertices[offset];    
      ry = vVertices[offset + 1];    

      offset += 4;

      sprintf(buff, "vertex%i x %f, y %f", i, rx, ry);
      render_text(context, buff, -1 + 8 * sx, 1 - stride - 50 * sy,    sx, sy);
    }

    stride += 0.08;
    sprintf(buff, "using rotate matrice:");
    render_text(context, buff, -1 + 8 * sx, 1 - stride - 50 * sy,    sx, sy);
    stride += 0.04;
    sprintf(buff, "---------------------------");
    render_text(context, buff, -1 + 8 * sx, 1 - stride - 50 * sy,    sx, sy);
    stride += 0.08;
    sprintf(buff, "{Cos(a), -Sin(a), 0, 0");
    render_text(context, buff, -1 + 8 * sx, 1 - stride - 50 * sy,    sx, sy);
    stride += 0.04;
    sprintf(buff, " Sin(a),  Cos(a), 0, 0");
    render_text(context, buff, -1 + 8 * sx, 1 - stride - 50 * sy,    sx, sy);
    stride += 0.04;
    sprintf(buff, "      0,       0, 1, 0");
    render_text(context, buff, -1 + 8 * sx, 1 - stride - 50 * sy,    sx, sy);
    stride += 0.04;
    sprintf(buff, "      0,       0, 0, 1}");
    render_text(context, buff, -1 + 8 * sx, 1 - stride - 50 * sy,    sx, sy);

    stride += 0.08;
    sprintf(buff, "mouse x: %f", vars.mouseX);
    render_text(context, buff, -1 + 8 * sx, 1 - stride - 50 * sy,    sx, sy);

    stride += 0.04;
    sprintf(buff, "mouse y: %f", vars.mouseY);
    render_text(context, buff, -1 + 8 * sx, 1 - stride - 50 * sy,    sx, sy);

    stride += 0.04;
    sprintf(buff, "scale: %f", scale);
    render_text(context, buff, -1 + 8 * sx, 1 - stride - 50 * sy,    sx, sy);
  }

  FT_Set_Pixel_Sizes(context->freetype.face, 0, 458);
  stride = 0.2;
  sprintf(buff, " %i", vars.counter);
  render_text(context, buff, -1 + 8 * sx + 0.9, 1 - stride - 0.4 - 50 * sy,    sx, sy);

  ii++;
}
