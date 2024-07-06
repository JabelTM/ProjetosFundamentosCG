#include "Tile.h"

using namespace glm;

Tile::Tile(GLuint texID, float texPosX, float texPosY, float texSizeX, float texSizeY, vec3 pos, vec3 scale, Shader* shader, float ang, bool isSelectable)
    : texID(texID), texPosX(texPosX), texPosY(texPosY), texSizeX(texSizeX), texSizeY(texSizeY), pos(pos), scale(scale), shader(shader), ang(ang), isSelected(false), isSelectable(isSelectable)
{
    GLfloat vertices[] = {
        // Triângulo 2
        -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, texPosX, texPosY,
         0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, texPosX + texSizeX, texPosY,
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f, texPosX + texSizeX, texPosY + texSizeY,
        // Triângulo 3
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f, texPosX + texSizeX, texPosY + texSizeY,
        -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f, texPosX, texPosY + texSizeY,
        -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, texPosX, texPosY,
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

Tile::~Tile()
{
    glDeleteVertexArrays(1, &this->VAO);
}

vec3 Tile::getPosition() {
    return this->pos;
}

void Tile::setSelected(bool selected) {
    if (canSelect()) {
        this->isSelected = selected;
    }
}

void Tile::setShader(Shader* shader)
{
    this->shader = shader;
}

void Tile::draw()
{
    update();

	glBindTexture(GL_TEXTURE_2D, texID);

    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

bool Tile::canSelect() {
    return this->isSelectable;
}

void Tile::update()
{
    glm::mat4 model = glm::mat4(1);
    model = glm::translate(model, pos);
    model = glm::rotate(model, glm::radians(ang), vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);

    if (isSelected) {
        shader->setVec3("vertexColor", 0.5f, 0.7f, 1.0f);  // Filtro azul
    } else {
        shader->setVec3("vertexColor",1.0f, 1.0f, 1.0f);  // Sem filtro
    }

	// Atualiza a matriz de modelo no shader
    shader->setMat4("model", glm::value_ptr(model));
}
