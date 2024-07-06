#include <iostream>
#include <string>
#include <assert.h>
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include<stb_image.h>
#include "Shader.h"
#include "Tilemap.h"
#include "Sprite.h"

using namespace std;
using namespace glm;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

GLuint loadTexture(string texturePath, int id);
float calculateDeltaTime();

const GLuint WIDTH = 1024, HEIGHT = 768;

Tilemap* map;
Sprite* knight;

int main()
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Texturas", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;

	}

	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Compilando e buildando o programa de shader
	Shader shader("../shaders/tex.vs", "../shaders/tex.fs");
	
	GLuint knightTexID = loadTexture("../assets/player.png", 2);

	//Ativando o buffer de textura 0 da opengl
	glActiveTexture(GL_TEXTURE0);

	shader.Use();

	//Matriz de projeção paralela ortografica
	mat4 projection = ortho(0.0,1024.0,768.0,0.0,-1.0,1.0);
	shader.setMat4("projection", value_ptr(projection));

	shader.setInt("texBuffer", 0);

	//habilita transparencia pra mesclar camadas
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint knightStopTexID = loadTexture("../assets/knight/walk1.png", 2);
	GLuint knightWalk1TexID = loadTexture("../assets/knight/walk2.png", 3);
	GLuint knightWalk2TexID = loadTexture("../assets/knight/walk3.png", 4);
	GLuint knightWalk3TexID = loadTexture("../assets/knight/walk4.png", 5);
	GLuint knightWalk4TexID = loadTexture("../assets/knight/walk5.png", 6);
	GLuint knightWalk5TexID = loadTexture("../assets/knight/walk6.png", 7);

    map = new Tilemap("../assets/terrain.png", "map.txt", &shader, vec3(512.0f, 386.0f, 0.0f));

	knight = new Sprite({knightStopTexID, knightWalk1TexID, knightWalk2TexID,
	   							knightWalk3TexID, knightWalk4TexID, knightWalk5TexID},
	    						vec3(300.0, 530.0, 0.0),
		 						vec3(150.0, 150.0, 0.0),
		  						&shader, 0.2f, 0.0f, false);

	knight->staticMoveTo(map->getSelectedTile(), false);

	while (!glfwWindowShouldClose(window))
	{
		float deltaTime = calculateDeltaTime();

		glfwPollEvents();

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height); //unidades de tela: pixel
		
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

        map->draw();

		knight->draw(deltaTime);

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return 0;
}

float calculateDeltaTime()
{
    static float lastTime = glfwGetTime();
    float currentTime = glfwGetTime();
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    return deltaTime;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
    
    if (!knight->getIsMovingTo()) {
		if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
			map->moveSelectionNorth();
			vec3 nextPos = map->getSelectedTile();
			knight->moveTo(nextPos, knight->getMirrored());
		}
	
    	if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
			map->moveSelectionNortheast();
			vec3 nextPos = map->getSelectedTile();
			knight->moveTo(nextPos, false);
		}
        

    	if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
			map->moveSelectionNorthwest();
			vec3 nextPos = map->getSelectedTile();
			knight->moveTo(nextPos, true);
		}
	

    	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
			map->moveSelectionSouth();
			vec3 nextPos = map->getSelectedTile();
			knight->moveTo(nextPos, knight->getMirrored());
		}
        

    	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
			map->moveSelectionSouthwest();
			vec3 nextPos = map->getSelectedTile();
			knight->moveTo(nextPos, true);
		}
	

    	if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
			map->moveSelectionSoutheast();
			vec3 nextPos = map->getSelectedTile();
			knight->moveTo(nextPos, false);
		}
        

    	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
			map->moveSelectionEast();
			vec3 nextPos = map->getSelectedTile();
			knight->moveTo(nextPos, false);
		}
	

    	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
			map->moveSelectionWest();
			vec3 nextPos = map->getSelectedTile();
			knight->moveTo(nextPos, true);
		}
	}
        
}

GLuint loadTexture(string texturePath, int id)
{
	GLuint texID;

	// Gera o identificador da textura na mem�ria 
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	//Configura��o do par�metro WRAPPING nas coords s e t
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//Confiruga��o do par�metro FILTERING na minifica��o e magnifica��o da textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		if (nrChannels == 3) //jpg, bmp
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else //png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}

