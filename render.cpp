/*
 * Draw a triangle using the shader pair created in Init()
 */
void Draw(ESContext *esContext) {
  UserData *userData = (UserData*) esContext->userData;

  // Set the viewport
  glViewport(0, 0, esContext->width, esContext->height);

  // Clear the color buffer
  glClear(GL_COLOR_BUFFER_BIT);

  // Use the program object
  glUseProgram(userData->programObject);

  // Define skeleton.
  GLfloat skeleton[3][2] = {
    {45.0 + xAngle, 0.5 + xRadius},
    {0.0 + yAngle, 0.3 + yRadius},
    {90.0 + zAngle, 0.2 + zRadius},
  };

  int i = 0;

  GLfloat x = 0.0, y = 0.0, z = 1.0;

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

  mAngle = xAngle;

  if (tempAngle == 0.0 || tempAngle != mAngle) {
    mAngleSin = sin(mAngle * PI / 180);
    mAngleCos = cos(mAngle * PI / 180);
    tempAngle = mAngle;
  }

  GLfloat mRotate[] {
    mAngleCos, -mAngleSin,  0.0, 0.0,
   mAngleSin,  mAngleCos,  0.0, 0.0,
    0.0,        0.0,        1.0, 0.0,
    0.0,        0.0,        0.0, 1.0,
  };
  
  GLfloat vVertices[] = {
    -0.25, -0.25, 0.0, 1.0,
    0.25, -0.25, 0.0, 1.0,
    -0.25, 0.25, 0.0, 1.0,
    0.25, 0.25, 0.0, 1.0,
  };

  multipleMatrices4x4(vVertices, mRotate);

  GLfloat texCoords[4][2] = {
    {0, 0},
    {1, 0},
    {0, 1},
    {1, 1},
  };

  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof texCoords, texCoords, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof vVertices, vVertices, GL_DYNAMIC_DRAW);

  glUniform1i(uniform_istext, 0);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  
  if (debug) {
    float sx = 2.0 / esContext->width;
    float sy = 2.0 / esContext->height;

    glUniform1i(uniform_istext, 1);
    glUniform4f(uniform_color, 1.0, 1.0, 1.0, 1.0);

    float stride = -0.1, rx = 0.0, ry = 0.0;
    int offset = 0;
    char buff[100], message[100];

    for (int i = 0; i < 4; i++) {
      stride += 0.04;

      switch (i) {

        case 0:
          sprintf(buff, "rendered frames %i", ii);
          break;

        case 1:
          stride += 0.04;
          sprintf(buff, "angles:");
          render_text(buff, -1 + 8 * sx, 1 - stride - 50 * sy,    sx, sy);
          stride += 0.04;
          sprintf(buff, "---------------------------");
          render_text(buff, -1 + 8 * sx, 1 - stride - 50 * sy,    sx, sy);
          stride += 0.04;
          sprintf(buff, "angle a %f", mAngle);
          break;

        case 2:
          sprintf(buff, "sin(a) %f", mAngleSin);
          break;

        case 3:
          sprintf(buff, "cos(a) %f", mAngleCos);
          break;
      }

      render_text(buff, -1 + 8 * sx, 1 - stride - 50 * sy,    sx, sy);
    }

    stride += 0.08;
    sprintf(buff, "Vertices:");
    render_text(buff, -1 + 8 * sx, 1 - stride - 50 * sy,    sx, sy);
    stride += 0.04;
    sprintf(buff, "---------------------------");
    render_text(buff, -1 + 8 * sx, 1 - stride - 50 * sy,    sx, sy);

    for (int i = 0; i < 4; i++) {
      stride += 0.04;
      rx = vVertices[offset];    
      ry = vVertices[offset + 1];    

      offset += 4;

      sprintf(buff, "vertex%i x %f, y %f", i, rx, ry);
      render_text(buff, -1 + 8 * sx, 1 - stride - 50 * sy,    sx, sy);
    }

    stride += 0.08;
    sprintf(buff, "using rotate matrice:");
    render_text(buff, -1 + 8 * sx, 1 - stride - 50 * sy,    sx, sy);
    stride += 0.04;
    sprintf(buff, "---------------------------");
    render_text(buff, -1 + 8 * sx, 1 - stride - 50 * sy,    sx, sy);
    stride += 0.08;
    sprintf(buff, "{Cos(a), -Sin(a), 0, 0");
    render_text(buff, -1 + 8 * sx, 1 - stride - 50 * sy,    sx, sy);
    stride += 0.04;
    sprintf(buff, " Sin(a),  Cos(a), 0, 0");
    render_text(buff, -1 + 8 * sx, 1 - stride - 50 * sy,    sx, sy);
    stride += 0.04;
    sprintf(buff, "      0,       0, 1, 0");
    render_text(buff, -1 + 8 * sx, 1 - stride - 50 * sy,    sx, sy);
    stride += 0.04;
    sprintf(buff, "      0,       0, 0, 1}");
    render_text(buff, -1 + 8 * sx, 1 - stride - 50 * sy,    sx, sy);
  }

  ii++;
}
