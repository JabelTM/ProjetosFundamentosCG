#include "Shader.h"
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

using namespace glm;
using namespace std;

class Tile {
public:
    Tile(GLuint texID, float texPosX, float texPosY, float texSizeX, float texSizeY, vec3 pos, vec3 scale, Shader* shader, float ang, bool isSelectable, bool hasIten, GLuint itenTexID, vec3 itenScale);
    ~Tile();

    void draw();
    void setShader(Shader* shader);
    void update(vec3 scale);
    void setSelected(bool selected);
    vec3 getPosition();
    bool canSelect();

private:
    GLuint texID;
    GLuint itenTexID;
    GLuint VAO;
    GLuint ITEN_VAO;
    vec3 pos;
    vec3 scale;
    vec3 itenScale;
    Shader* shader;
    float ang;
    float texPosX, texPosY, texSizeX, texSizeY;
    bool isSelected;
    bool isSelectable;
    bool hasIten;

    void setupTile();
    void setupIten();
    void draw(GLuint texID, GLuint VAO, vec3 scale);
};

