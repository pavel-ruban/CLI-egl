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
#include "context.hpp"
#include "common.hpp"
#include <streambuf>
#include <iostream>

extern bool run, debug;

float mAngle = 0.0;
float mAngleSin = 0.0;
float mAngleCos = 0.0;
float tempAngle = 0.0;

GLint uniform_istext, uniform_color;
GLuint vbo[2];

float xAngle = 0.0, xRadius = 0.0, yAngle = 0.0, yRadius = 0.0, zAngle = 0.0, zRadius = 0.0;
float xCustom = -1.0, yCustom = 1.0, zCustom = 0.0, wCustom = 0.0;

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
  UserData *userData = (UserData*) esContext->userData;
  
  // Init free type.
  if(FT_Init_FreeType(&userData->freetype.ft)) {
    fprintf(stderr, "Could not init freetype library\n");
    return 1;
  }
  if(FT_New_Face(userData->freetype.ft, "UbuntuMono-R.ttf", 0, &userData->freetype.face)) {
    fprintf(stderr, "Could not open font\n");
    return 1;
  } 

  userData->freetype.g = userData->freetype.face->glyph; 

  FT_Set_Pixel_Sizes(userData->freetype.face, 0, 18);

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
