/* Hello Triangle - c�digo adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle 
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gr�fico - Unisinos
 * Vers�o inicial: 7/4/2017
 * �ltima atualiza��o em 14/08/2023
 *
 */

#include <iostream>
#include <string>
#include <assert.h>

//GLM
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//STB IMAGE
#include<stb_image.h>

//Classe para manipula��o dos shaders
#include "Shader.h"

//Classe para criar e manipular sprites
#include "Sprite.h"

using namespace std;
using namespace glm;

// Prot�tipo da fun��o de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Prot�tipos das fun��es
GLuint loadTexture(string texturePath, int id);
float calculateDeltaTime();

// Dimens�es da janela (pode ser alterado em tempo de execu��o)
const GLuint WIDTH = 1024, HEIGHT = 768;

Sprite* knight;

// Fun��o MAIN
int main()
{
	// Inicializa��o da GLFW
	glfwInit();

	// Cria��o da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Texturas", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da fun��o de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros d fun��es da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;

	}

	// Obtendo as informa��es de vers�o
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Compilando e buildando o programa de shader
	Shader shader("../shaders/tex.vs", "../shaders/tex.fs");
	
	GLuint backgroundTexID = loadTexture("../assets/Battleground3.png", 1);
	GLuint knightStopTexID = loadTexture("../assets/knight/walk1.png", 2);
	GLuint knightWalk1TexID = loadTexture("../assets/knight/walk2.png", 3);
	GLuint knightWalk2TexID = loadTexture("../assets/knight/walk3.png", 4);
	GLuint knightWalk3TexID = loadTexture("../assets/knight/walk4.png", 5);
	GLuint knightWalk4TexID = loadTexture("../assets/knight/walk5.png", 6);
	GLuint knightWalk5TexID = loadTexture("../assets/knight/walk6.png", 7);
	GLuint knightJump2TexID = loadTexture("../assets/knight/jump/jump2.png", 9);
	GLuint knightJump3TexID = loadTexture("../assets/knight/jump/jump3.png", 10);
	GLuint knightJump5TexID = loadTexture("../assets/knight/jump/jump5.png", 12);
	GLuint knightJump6TexID = loadTexture("../assets/knight/jump/jump6.png", 13);

	//Ativando o buffer de textura 0 da opengl
	glActiveTexture(GL_TEXTURE0);

	shader.Use();

	//Matriz de projeção paralela ortografica
	glm::mat4 projection = glm::ortho(0.0,1024.0,768.0,0.0,-1.0,1.0);
	//Enviando para o shader a matriz como uma var uniform
	shader.setMat4("projection", glm::value_ptr(projection));

	shader.setInt("texBuffer", 0);

	//habilita transparencia pra mesclar camadas
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Sprite background({backgroundTexID},
	   				vec3(512.0f, 386.0f, 0.0f),
	    			vec3(1024.0, 768.0, 0.0),
		 			&shader, 0.2f, 0.0f, false);
	knight = new Sprite({knightStopTexID, knightWalk1TexID, knightWalk2TexID,
	   							knightWalk3TexID, knightWalk4TexID, knightWalk5TexID},
	    						vec3(300.0, 530.0, 0.0),
		 						vec3(150.0, 150.0, 0.0),
		  						&shader, 0.2f, 0.0f, false);
	knight->enableJump(true);
	knight->setJumpTextures({knightJump2TexID, knightJump3TexID,
							knightJump5TexID, knightJump6TexID});
	knight->setJumpFrameTime(2.0f);
	knight->setJumpSpeed(0.4f);
	knight->setJumpHeight(150.0f);


	// Loop da aplica��o - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		float deltaTime = calculateDeltaTime();

		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as fun��es de callback correspondentes
		glfwPollEvents();

		// Definindo as dimens�es da viewport com as mesmas dimens�es da janela da aplica��o
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height); //unidades de tela: pixel
		
		// Limpa o buffer de cor
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		background.draw(deltaTime);

		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
			knight->moveRight();
        } else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
			knight->moveLeft();
        } else
		{
			knight->stopMoving();
		}
		
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
	
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		knight->startJump();
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

