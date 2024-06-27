#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "Shader.h"

using namespace glm;

#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "Shader.h"

using namespace glm;

class Sprite {
public:
    Sprite(std::vector<GLuint> texIDs, vec3 pos, vec3 scale, Shader* shader, float speed, float ang, bool mirrored);
    ~Sprite();

    void draw(float deltaTime);
    void moveLeft();
    void moveRight();
    void stopMoving();
    void mirrorIt(bool mirrored);
    void setShader(Shader* shader);

    void enableJump(bool enable);
    void startJump();
    void setJumpSpeed(float speed);
    void setJumpHeight(float height);
    void setJumpOrigin(float origin);
    void setJumpFrameTime(float frameTime);
    void setJumpTextures(std::vector<GLuint> texIDs);
    void update(float deltaTime);

private:
    std::vector<GLuint> texIDs;
    GLuint VAO;
    vec3 pos;
    vec3 scale;
    Shader* shader;
    float speed;
    float ang;
    bool isMirrored;
    bool isMoving;
    bool canJump;  // Indica se o pulo está habilitado
    bool isJumping;
    bool isGoingUp;
    std::vector<GLuint> jumpTexIDs;
    int currentFrame;
    float frameTime;
    float jumpFrameTime;
    float timeAccumulator;

    // Variáveis para controle de pulo
    float jumpSpeed;
    float jumpHeight;
    float jumpOrigin;

    void updateTexture(float deltaTime);
    void updateJump(float deltaTime);
};

