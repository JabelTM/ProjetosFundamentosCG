#include "Sprite.h"

Sprite::Sprite(std::vector<GLuint> texIDs, vec3 pos, vec3 scale, Shader* shader, float speed, float ang, bool mirrored)
    : texIDs(texIDs), pos(pos), scale(scale), shader(shader), speed(speed), ang(ang), isMirrored(mirrored),
      isMoving(false), canJump(false), isJumping(false), currentFrame(0), frameTime(0.1f), jumpFrameTime(1.0f), timeAccumulator(0.0f),
      jumpSpeed(1.0f), jumpHeight(100.0f), jumpOrigin(pos.y), isMovingTo(false)
{
    this->pos = pos;
    this->scale = scale;
    this->shader = shader;
    this->speed = speed;
    this->ang = ang;
    mirrorIt(mirrored);

	GLfloat vertices[] = {
    // Triângulo 2
    -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
     0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    // Triângulo 3
     0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
};

    GLuint VBO;

    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &this->VAO);

    glBindVertexArray(this->VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

Sprite::~Sprite()
{
    glDeleteVertexArrays(1, &this->VAO);
}

void Sprite::setShader(Shader* shader)
{
    this->shader = shader;
}

void Sprite::draw(float deltaTime)
{
    update(deltaTime);

	if (isJumping) {
		glBindTexture(GL_TEXTURE_2D, jumpTexIDs[currentFrame]);
	} else {
		glBindTexture(GL_TEXTURE_2D, texIDs[currentFrame]);
	}
    
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Sprite::mirrorIt(bool mirrored)
{
    isMirrored = mirrored;
}

void Sprite::moveLeft()
{
    pos.x -= speed;
    mirrorIt(true);
    isMoving = true;
}

void Sprite::moveRight()
{
    pos.x += speed;
    mirrorIt(false);
    isMoving = true;
}

void Sprite::stopMoving()
{
    isMoving = false;
    currentFrame = 0; // Volta para a textura original
}

void Sprite::enableJump(bool enable)
{
    canJump = enable;
    if (!canJump) {
        isJumping = false;
    }
}

void Sprite::startJump()
{
    if (canJump) {
        isJumping = true;
		isGoingUp = true;
		currentFrame = 0;
    }
}

void Sprite::setJumpSpeed(float speed)
{
    jumpSpeed = speed;
}

void Sprite::setJumpHeight(float height)
{
    jumpHeight = height;
}

void Sprite::setJumpOrigin(float origin)
{
    jumpOrigin = origin;
}

void Sprite::setJumpTextures(std::vector<GLuint> texIDs)
{
	jumpTexIDs = texIDs;
}

void Sprite::setJumpFrameTime(float frameTime) {
	jumpFrameTime = frameTime;
}

void Sprite::update(float deltaTime)
{
    glm::mat4 model = glm::mat4(1);

	// Atualiza posição vertical durante o pulo
    if (isJumping) {
        updateJump(deltaTime);
    }

    if (isMovingTo) {
        updatePosition();
    }
    
	
    // Aplica transformações de rotação, escala e espelhamento
    if (isMirrored) {
        model = glm::translate(model, vec3(pos.x - (scale.x / 2), pos.y, pos.z));
        model = glm::rotate(model, glm::radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
    } else {
        model = glm::translate(model, pos);
    }
    model = glm::rotate(model, glm::radians(ang), vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);

	// Atualiza a matriz de modelo no shader
    shader->setMat4("model", glm::value_ptr(model));

    updateTexture(deltaTime);
}

void Sprite::updatePosition() {
    if (pos.x == targetPosition.x && pos.y == targetPosition.y) {
        isMovingTo = false;
        return;
    }

    if (pos.x < targetPosition.x && pos.x + speed < targetPosition.x) {
        pos.x += speed;
    } else if (pos.x > targetPosition.x && pos.x - speed > targetPosition.x) {
        pos.x -=speed;
    } else {
        pos.x = targetPosition.x;
    }

    if (pos.y < targetPosition.y && pos.y + speed < targetPosition.y) {
        pos.y += speed;
    } else if (pos.y > targetPosition.y && pos.y - speed > targetPosition.y) {
        pos.y -=speed;
    } else {
        pos.y = targetPosition.y;
    }
    
    if (pos.x == targetPosition.x && pos.y == targetPosition.y) {
        currentFrame = 0;
    }
}

void Sprite::updateTexture(float deltaTime)
{
    // Lógica de animação da textura
    if ((isMoving && !isJumping) || isMovingTo) {
        timeAccumulator += deltaTime;
        if (timeAccumulator >= frameTime) {
            timeAccumulator -= frameTime;
            currentFrame = (currentFrame + 1) % texIDs.size();
        }
    } else if (isJumping) {
        if (isGoingUp && pos.y >= jumpOrigin - jumpHeight) {
            currentFrame = 0;
        } else if (isGoingUp) {
            currentFrame = 1;
        } else if (pos.y <= jumpOrigin - jumpHeight) {
            currentFrame = 2;
        } else {
            currentFrame = 3;
        }
    }
}

void Sprite::updateJump(float deltaTime)
{
    // Verifica se atingiu a altura máxima do pulo
	if (isGoingUp) {
		pos.y -= jumpSpeed;
	} else {
		pos.y += jumpSpeed;
	}

	if (pos.y <= jumpOrigin - jumpHeight) {
		isGoingUp = false;
	}

    if (pos.y >= jumpOrigin) {
		pos.y = jumpOrigin;
		isJumping = false;
		currentFrame = 0;
    }
}

bool Sprite::getMirrored() {
    return isMirrored;
}

void Sprite::moveTo(vec3 targetPosition, bool mirror) { 
    mirrorIt(mirror);
    float adjustX = 4.0f;
    if (mirror)
        adjustX = 2.2f;
    
    this->isMovingTo = true;

    this->targetPosition = vec3(targetPosition.x + (scale.x / adjustX), targetPosition.y - (scale.y / 4.0f), targetPosition.z);
}

void Sprite::staticMoveTo(vec3 targetPosition, bool mirror) {
    mirrorIt(mirror);
    float adjustX = 4.0f;
    if (mirror)
        adjustX = 2.2f;
    
    this->pos = vec3(targetPosition.x + (scale.x / adjustX), targetPosition.y - (scale.y / 4.0f), targetPosition.z);
}

bool Sprite::getIsMovingTo() {
    return this->isMovingTo;
}