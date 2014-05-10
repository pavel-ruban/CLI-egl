#include <stdio.h>
#include <string>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/egl.h>
#include <vector>

using namespace std;

vector < vector < GLfloat > > identity {
  {1.0, 0.0, 0.0, 0.0},
    {0.0, 1.0, 0.0, 0.0},
    {0.0, 0.0, 1.0, 0.0},
    {0.0, 0.0, 0.0, 1.0},
};

class bone {
  public:
    string name;
    GLint weight;  
    bone* parent;
    vector< vector< GLfloat > > matrix;

    // Constructors.
    bone();
    bone(int);
    bone(string);
    bone(bone*, int = 0);
    bone(vector < vector < GLfloat > >, int = 0);
    bone(bone*, vector < vector < GLfloat > >, int = 0);
    bone(string, bone*, int = 0);
    bone(string, bone*, vector < vector < GLfloat > >, int = 0);

    // Add matrix to bone matrix.
    void multipleMatrix (vector < vector < GLfloat > > mMatrix) {
      vector < GLfloat > row(4, 0.0);
      int offset = 0, cell = 0;

      for (int i = 0; i < 4; i++) {
        cell = 0;
        for (int i = 0; i < 4; i++) {
          row[cell] =
            matrix[offset][0] * mMatrix[0][cell] +
            matrix[offset][1] * mMatrix[1][cell] +
            matrix[offset][2] * mMatrix[2][cell] +
            matrix[offset][3] * mMatrix[3][cell];
            
          cell++;
        }

        // Save multiplied left operand row to base matrix. As current vector data won't be used anymore & could
        // be safely changed.
        matrix[offset] = row;
        offset++;
      }
    }

};

// Different constructor's definitions.
bone::bone() {
  matrix = identity;

  name = "noname";
  parent = 0; 
  weight = 0; 
}

bone::bone(int weight) {
  parent = 0;
  name = "noname";
  bone::weight = weight;

  matrix = identity;
}

bone::bone(string name) {
  parent = 0;
  bone::name = name;
  bone::weight = 0;

  matrix = identity;
}

bone::bone(bone* bone, int weight) {
  parent = bone;
  name = "noname";
  bone::weight = weight;

  matrix = identity;
}

bone::bone(vector < vector < GLfloat > > matrix, int weight) {
  bone::matrix = matrix;
  bone::weight = weight;
  name = "noname";
  parent = 0;
}

bone::bone(string name, bone* bone, int weight) {
  bone::name = name;
  parent = bone;
  bone::matrix = identity;
  bone::weight = weight;
}

bone::bone(string name, bone* bone, vector < vector < GLfloat > > matrix, int weight) {
  bone::name = name;
  parent = bone;
  bone::matrix = matrix;
  bone::weight = weight;
}

void dump_matrix(vector< vector< GLfloat > >* matrix, int tab = 0) {
  for (int i = 0; i < 4; i++) {
    int ii = 0;

    while (ii < tab && tab != 0) {
      printf(" ");
      ii++;
    }

    for (int ii = 0; ii < 4; ii++) {
      printf("%f, ", (*matrix)[i][ii]);
    }

    printf("\n");
  }
}

void dump_bone(bone* bone) {
  static int i = 0;
  printf("bone \"%s\"\n", bone->name.c_str());
  printf("  weight: %i\n", i, bone->weight);
  printf("  matrix:\n\n");
  
  dump_matrix(&bone->matrix, 2);
  printf("\n");

  string parent;
  if (bone->parent == 0) {
    parent = "root bone";
  }
  else {
    parent = string("parent: ") + bone->parent->name;
  }

  printf("  %s", parent.c_str());
  printf("\n");

  i++;
}

int main(int argc, char* argv[]) {
  using namespace std;
  bone first(string("root"));
  //first.weight = 4;
  //first.parent = 0;

  vector < vector < GLfloat > > matrix = {
    {3.0, 0.0, 0.0, 0.0},
    {0.0, 1.0, 0.0, 0.0},
    {0.0, 0.0, 1.0, 0.0},
    {0.0, 0.0, 0.0, 1.0},
  };

  bone second(string("body"), &first);
  second.multipleMatrix(matrix);
  second.multipleMatrix(matrix);
  //second.weight = 3;
  //first.matrix[2][3] = 1.1;

  dump_matrix(&first.matrix);

  printf("\n");

  dump_matrix(&second.matrix);

  printf("\n");
  dump_bone(&first);
  printf("\n");
  dump_bone(&second);
  // printf("\n%i ", second.weight);

  return 0;
}
