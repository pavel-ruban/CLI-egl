/**
 * Current header provider skeleton structure declarations
 * in order to make an animation skeleton.
 */

#include "common.hpp"

class bone {
  public:
    std::string name;
    GLint weight;  
    bone* parent;
    // Skeleton matrix.
    std::vector< std::vector< GLfloat > > matrix;

    // Constructors.
    bone();
    bone(int);
    bone(string);
    bone(bone*, int = 0);
    bone(std::vector < std::vector < GLfloat > >, int = 0);
    bone(bone*, std::vector < std::vector < GLfloat > >, int = 0);
    bone(string, bone*, int = 0);
    bone(string, bone*, std::vector < std::vector < GLfloat > >, int = 0);

    // Add matrix to bone matrix.
    void multipleMatrix (std::vector < std::vector < GLfloat > > mMatrix);
};
