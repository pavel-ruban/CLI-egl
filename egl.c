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
#include <stdlib.h>
#include "esUtil.h"

typedef struct
{
  // Handle to a program object
  GLuint programObject;

} UserData;

///
// Create a shader object, load the shader source, and
// compile the shader.
//
GLuint LoadShader(GLenum type, const char *shaderSrc)
{
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

  if (!compiled) 
  {
    GLint infoLen = 0;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

    if (infoLen > 1)
    {
      char* infoLog = malloc (sizeof(char) * infoLen);

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
int Init(ESContext *esContext)
{
  esContext->userData = malloc(sizeof(UserData));

  UserData *userData = esContext->userData;
  GLbyte vShaderStr[] =  
    "attribute vec4 vPosition;    \n"
    "precision mediump float; \n"
    //"attribute vec4 v_vector;    \n"
    "void main()                  \n"
    "{                            \n"
    " mat4 theMat;    \n"
    " theMat[0] = vec4(1.0, 0.0, 0.0, 0.0);    \n"
    " theMat[1] = vec4(0.0, 1.0, 0.0, 0.0);    \n"
    " theMat[2] = vec4(0.0, 0.0, 1.0, 0.0);    \n"
    " theMat[3] = vec4(-0.5, 0.0, 0.0, 1.0);    \n"
    "  gl_Position = vPosition * theMat;  \n"
    //     "  if (v_vector[0] == 1.0) {                            \n"
    //     "    gl_Position[0] = v_vector[2] * cos(v_vector[1]);   \n"
    //     "    gl_Position[1] = v_vector[2] * sin(v_vector[1]);   \n"
    //     "    gl_Position[2] = 0.0;                           \n"
    //     "  }                            \n"
    "}                            \n";

  GLbyte fShaderStr[] =  
    "precision mediump float;\n"\
    "void main()                                  \n"
    "{                                            \n"
    "  gl_FragColor = vec4 ( 0.0, 1.0, 1.0, 1.0);\n"
    "}                                            \n";

  GLuint vertexShader;
  GLuint fragmentShader;
  GLuint programObject;
  GLint linked;

  // Load the vertex/fragment shaders
  vertexShader = LoadShader(GL_VERTEX_SHADER, vShaderStr);
  fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fShaderStr);

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

  if (!linked) 
  {
    GLint infoLen = 0;

    glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);

    if (infoLen > 1)
    {
      char* infoLog = malloc (sizeof(char) * infoLen);

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
  return GL_TRUE;
}

/*
 * Draw a triangle using the shader pair created in Init()
 */
void Draw(ESContext *esContext) {
  UserData *userData = esContext->userData;

  // Set the viewport
  glViewport(0, 0, esContext->width, esContext->height);

  // Clear the color buffer
  glClear(GL_COLOR_BUFFER_BIT);

  // Use the program object
  glUseProgram(userData->programObject);

  GLfloat vVertices[] = {
    0.0f, 0.0f, 0.0f, 
    45.0f, 0.5f, 0.0f, 
    270.0f, 0.3f, 0.0f,
  };

  // Load the vertex data
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
  glEnableVertexAttribArray(0);

//  GLuint vbo_direction_vectors;
//  GLint attribute_v_vector;
//
//  static float i = 0.0;
//  i = i + 0.01;
//  if (i > 360.0)
//    i = 0.0;
//
//  GLfloat direction_vetors[] = {
//    0.0, 0.0, 0.0, 0.0,
//    1.0, i, 1.0, 0.0,
//  };
//  glGenBuffers(1, &vbo_direction_vectors);
//  glBindBuffer(GL_ARRAY_BUFFER, vbo_direction_vectors);
//  glBufferData(GL_ARRAY_BUFFER, sizeof(direction_vetors), direction_vetors, GL_STATIC_DRAW);
//
//  const char* attribute_name = "v_vector";
//  attribute_v_vector = glGetAttribLocation(userData->programObject, attribute_name);
//  if (attribute_v_vector == -1) {
//    return;
//  }
//
//  glEnableVertexAttribArray(attribute_v_vector);
//  glBindBuffer(GL_ARRAY_BUFFER, vbo_direction_vectors);
//  glVertexAttribPointer(
//      attribute_v_vector, // attribute
//      4,                 // number of elements per vertex, here (r,g,b)
//      GL_FLOAT,          // the type of each element
//      GL_FALSE,          // take our values as-is
//      0,                 // no extra data between each position
//      0                  // offset of first element
//     );

  glDrawArrays(GL_LINE_LOOP, 0, 3);
  glDrawArrays(GL_LINES, 3, 2);
  glDrawArrays(GL_LINES, 5, 2);
}

int main(int argc, char *argv[])
{
  ESContext esContext;
  UserData  userData;

  esInitContext(&esContext);
  esContext.userData = &userData;

  esCreateWindow(&esContext, "Hello Triangle", 400, 400, ES_WINDOW_RGB);

  if (!Init(&esContext))
    return 0;

  esRegisterDrawFunc(&esContext, Draw);

  esMainLoop(&esContext);
}
