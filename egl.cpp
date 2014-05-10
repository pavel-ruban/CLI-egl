//
// Book:      OpenGL(R) ES 2.0 Programming Guide
// Authors:   Aaftab Munshi, Dan Ginsburg, Dave Shreiner
// ISBN-10:   0321502795
// ISBN-13:   9780321502797
// Publisher: Addison-Wesley Professional
// URLs:      http://safari.informit.com/9780321563835
//            http://www.opengles-book.com
//

// Hello_Triangle.c
//
//    This is a simple example that draws a single triangle with
//    a minimal vertex/fragment shader.  The purpose of this 
//    example is to demonstrate the basic concepts of 
//    OpenGL ES 2.0 rendering.
#include "esUtil.h"
#include <fstream>
#include <streambuf>
#include <iostream>
#include <string>
#include "input.h"
#include <ft2build.h>
#include FT_FREETYPE_H

bool run = true, debug = true;

unsigned int ii = 0;
float mAngle = 0.0;
float mAngleSin = 0.0;
float mAngleCos = 0.0;
float tempAngle = 0.0;
FT_Library ft;
FT_Face face;
FT_GlyphSlot g;

GLint uniform_istext, uniform_color;
GLuint vbo[2];

float xAngle = 0.0, xRadius = 0.0, yAngle = 0.0, yRadius = 0.0, zAngle = 0.0, zRadius = 0.0;
float xCustom = -1.0, yCustom = 1.0, zCustom = 0.0, wCustom = 0.0;

typedef struct {
  // Handle to a program object
  GLuint programObject;

} UserData;

///
// Create a shader object, load the shader source, and
// compile the shader.
//
GLuint LoadShader(GLenum type, const char *shaderSrc) {
  GLuint shader;
  GLint compiled;

  // Create the shader object
  shader = glCreateShader(type);

  if (shader == 0)
    return 0;

  // Load the shader source
  glShaderSource(shader, 1, &shaderSrc, NULL);

  // Compile the shader
  glCompileShader(shader);

  // Check the compile status
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

  if (!compiled) {
    GLint infoLen = 0;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

    if (infoLen > 1) {
      char* infoLog = (char*) malloc(sizeof(char) * infoLen);

      glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
      esLogMessage("Error compiling shader:\n%s\n", infoLog);            

      free(infoLog);
    }

    glDeleteShader(shader);
    return 0;
  }

  return shader;

}

///
// Initialize the shader and program object
//
int Init(ESContext *esContext) {
  esContext->userData = malloc(sizeof(UserData));

  UserData *userData = (UserData*) esContext->userData;

  std::ifstream vsource("vertex_shader.glsl");
  std::string v_str(
    (std::istreambuf_iterator<char>(vsource)),
    std::istreambuf_iterator<char>()
  );

  GLbyte* vShaderStr = (GLbyte*) v_str.c_str();

  std::ifstream fsource("fragment_shader.glsl");
  std::string f_str(
    (std::istreambuf_iterator<char>(fsource)),
    std::istreambuf_iterator<char>()
  );

  GLbyte* fShaderStr = (GLbyte*) f_str.c_str(); 

  GLuint vertexShader;
  GLuint fragmentShader;
  GLuint programObject;
  GLint linked;

  // Load the vertex/fragment shaders
  vertexShader = LoadShader(GL_VERTEX_SHADER, (const char*) vShaderStr);
  fragmentShader = LoadShader(GL_FRAGMENT_SHADER, (const char*) fShaderStr);

  // Create the program object
  programObject = glCreateProgram();

  if (programObject == 0)
    return 0;

  glAttachShader(programObject, vertexShader);
  glAttachShader(programObject, fragmentShader);

  // Bind vPosition to attribute 0   
  glBindAttribLocation(programObject, 0, "vPosition");

  // Link the program
  glLinkProgram(programObject);

  // Check the link status
  glGetProgramiv(programObject, GL_LINK_STATUS, &linked);

  if (!linked) {
    GLint infoLen = 0;

    glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);

    if (infoLen > 1) {
      char* infoLog = (char*) malloc(sizeof(char) * infoLen);

      glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
      esLogMessage("Error linking program:\n%s\n", infoLog);            

      free(infoLog);
    }

    glDeleteProgram(programObject);
    return GL_FALSE;
  }

  // Store the program object
  userData->programObject = programObject;

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Prepare uniforms.
  GLint maxUniformLen;
  GLint numUniforms;
  char *uniformName;
  GLint index;

  glGetProgramiv(programObject, GL_ACTIVE_UNIFORMS, &numUniforms);
  glGetProgramiv(programObject, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformLen);

  uniformName = (char*) malloc(sizeof(char) * maxUniformLen);
  GLfloat green[] = {1.0, 1.0, 1.0, 0.0};
        GLuint tex;
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
  for (index = 0; index < numUniforms; index++) {
    GLint size;
    GLenum type;
    GLint location;

    glGetActiveUniform(programObject, index, maxUniformLen, NULL, &size, &type, uniformName);

    location = glGetUniformLocation(programObject, uniformName);
     
    switch(type) {
      case 35678:
        glUniform1i(location, 0);
        break;

      case GL_INT:
        uniform_istext = location;
        break;
      
    }
  }

  GLint location;
  
  if ((location = glGetUniformLocation(programObject, "color")) != -1)
    uniform_color = location;
    
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  
  glGenBuffers(2, vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

  return GL_TRUE;
}

void render_text(const char *text, float x, float y, float sx, float sy) {
  const char *p;
 
  for(p = text; *p; p++) {
    if(FT_Load_Char(face, *p, FT_LOAD_RENDER))
        continue;
 
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
 
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
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

// Opengl loop.
int graphic_thread() {
  ESContext esContext;
  UserData  userData;

  // Init free type.

  if(FT_Init_FreeType(&ft)) {
    fprintf(stderr, "Could not init freetype library\n");
    return 1;
  }
  if(FT_New_Face(ft, "UbuntuMono-R.ttf", 0, &face)) {
    fprintf(stderr, "Could not open font\n");
    return 1;
  } 

  g = face->glyph; 

  FT_Set_Pixel_Sizes(face, 0, 18);

  esInitContext(&esContext);
  esContext.userData = &userData;

  esCreateWindow(&esContext, "Hello Triangle", 1080, 1080, ES_WINDOW_RGB);

  if (!Init(&esContext))
    return 0;

  esRegisterDrawFunc(&esContext, Draw);

  esMainLoop(&esContext);
}

extern void event_thread();

int main(int argc, char *argv[]) {
  // Split program into two separate threads.
  boost::thread eventThread(event_thread);
  boost::thread graphicThread(graphic_thread);

  eventThread.join();
  graphicThread.join();

  return 0;
}
