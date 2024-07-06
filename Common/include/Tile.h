#include "Shader.h"
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

using namespace glm;
using namespace std;

class Tile {
public:
    Tile(GLuint texID, float texPosX, float texPosY, float texSizeX, float texSizeY, vec3 pos, vec3 scale, Shader* shader, float ang, bool isSelectable);
    ~Tile();

    void draw();
    void setShader(Shader* shader);
    void update();
    void setSelected(bool selected);
    vec3 getPosition();
    bool canSelect();

private:
    GLuint texID;
    GLuint VAO;
    vec3 pos;
    vec3 scale;
    Shader* shader;
    float ang;
    float texPosX, texPosY, texSizeX, texSizeY;
    bool isSelected;
    bool isSelectable;
};

