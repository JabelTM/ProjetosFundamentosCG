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


using namespace std;
//using namespace glm; //para n�o usar glm::

//Classe para manipula��o dos shaders
#include "Shader.h"

// Prot�tipo da fun��o de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Prot�tipos das fun��es
int setupGeometryBackground();
int setupGeometryKnight();
void drawScene(int VAO, int texID);
GLuint loadTexture(string texturePath, int id);
int verifyCollision(GLfloat vertice, float transl);
void walkRight(float deslocation);
void walkLeft(float deslocation);

// Dimens�es da janela (pode ser alterado em tempo de execu��o)
const GLuint WIDTH = 1024, HEIGHT = 768;

//coordenadas de textura olhando para a direita
GLfloat verticesKnight[] = {
		//Triangulo 2
		-0.3125, 0.1166, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0,
		0.0125, 0.1166, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0,
		0.0125, 0.5166, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0,
		//Triangulo 3
		0.0125, 0.5166, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0,
		-0.3125, 0.5166, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0,
		-0.3125, 0.1166, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0,
	};

//coordenadas de textura olhando para a esquerda
// GLfloat verticesKnight[] = {
// 		//Triangulo 2
// 		-0.3125, 0.1166, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0,
// 		0.0125, 0.1166, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
// 		0.0125, 0.5166, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0,
// 		//Triangulo 3
// 		0.0125, 0.5166, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0,
// 		-0.3125, 0.5166, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0,
// 		-0.3125, 0.1166, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0,
// 	};

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
	//Shader shader("../shaders/helloTriangle.vs", "../shaders/helloTriangle.fs");
	Shader shader("../shaders/tex.vs", "../shaders/tex.fs");

	// Gerando um buffer simples, com a geometria
	GLuint VAO_BACKGROUND = setupGeometryBackground();
	GLuint VAO_KNIGHT = setupGeometryKnight();
	
	GLuint texID = loadTexture("../assets/Battleground3.png", 1);
	GLuint texID2 = loadTexture("../assets/knight/walk1.png", 2);
	GLuint texID3 = loadTexture("../assets/knight/walk2.png", 3);
	GLuint texID4 = loadTexture("../assets/knight/walk3.png", 4);
	GLuint texID5 = loadTexture("../assets/knight/walk4.png", 5);
	GLuint texID6 = loadTexture("../assets/knight/walk5.png", 6);
	GLuint texID7 = loadTexture("../assets/knight/walk6.png", 7);
	
	//Ativando o buffer de textura 0 da opengl
	glActiveTexture(GL_TEXTURE0);

	shader.Use();

	//Matriz de projeção paralela ortografica
	glm::mat4 projection = glm::ortho(0.0,1024.0,768.0,0.0,-1.0,1.0);
	//Enviando para o shader a matriz como uma var uniform
	shader.setMat4("projection", glm::value_ptr(projection));
	
	//Matriz de transformaço do objeto (matriz de modelo)
	glm::mat4 model = glm::mat4(1); //matriz identidade
	model = glm::translate(model, glm::vec3(512.0, 386.0, 0.0));
	// model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0));
	model = glm::scale(model, glm::vec3(512.0, 386.0, 0.0));
	shader.setMat4("model", glm::value_ptr(model));

	shader.setInt("texBuffer", 0);

	int mooving = 0;
	int knightTex = 1;
	float deslocation = 0.0003f;

	//habilita transparencia pra mesclar camadas
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Loop da aplica��o - "game loop"
	while (!glfwWindowShouldClose(window))
	{
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

		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            if (verifyCollision(verticesKnight[8], deslocation) == 0)
			{
				walkRight(deslocation);
				mooving = 1;
				VAO_KNIGHT = setupGeometryKnight();
			}
        }

		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            if (verifyCollision(verticesKnight[0], -deslocation) == 0)
			{
				walkLeft(deslocation);
				mooving = 1;
				VAO_KNIGHT = setupGeometryKnight();
			}
        }

		//Chamadas de desenho da cena
		drawScene(VAO_BACKGROUND, texID);
		
		if (mooving == 1) {
			if (knightTex < 150)
			{
				drawScene(VAO_KNIGHT, texID2);
				knightTex++;
			} else if (knightTex >= 150 && knightTex < 300)
			{
				drawScene(VAO_KNIGHT, texID3);
				knightTex++;
			} else if (knightTex >= 300 && knightTex < 450)
			{
				drawScene(VAO_KNIGHT, texID4);
				knightTex++;
			} else if (knightTex >= 450 && knightTex < 600)
			{
				drawScene(VAO_KNIGHT, texID5);
				knightTex++;
			} else if (knightTex >= 600 && knightTex < 750)
			{
				drawScene(VAO_KNIGHT, texID6);
				knightTex++;
			} else {
				drawScene(VAO_KNIGHT, texID7);
				if (knightTex == 900)
				{
					knightTex = 1;
				} else {
					knightTex++;
				}
			}
		} else {
			drawScene(VAO_KNIGHT, texID2);
		}

		// Troca os buffers da tela
		glfwSwapBuffers(window);
		mooving = 0;
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO_BACKGROUND);
	glDeleteVertexArrays(2, &VAO_KNIGHT);
	// glDeleteVertexArrays(3, &VAO3);
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

void walkRight(float deslocation) {
	for (int i = 0; i < sizeof(verticesKnight); i+=8)
	{
		verticesKnight[i] += deslocation;
	}
	verticesKnight[6] = 0.0;
	verticesKnight[7] = 0.0;
	verticesKnight[14] = 1.0;
	verticesKnight[15] = 0.0;
	verticesKnight[22] = 1.0;
	verticesKnight[23] = 1.0;
	verticesKnight[30] = 1.0;
	verticesKnight[31] = 1.0;
	verticesKnight[38] = 0.0;
	verticesKnight[39] = 1.0;
	verticesKnight[46] = 0.0;
	verticesKnight[47] = 0.0;
}

void walkLeft(float deslocation) {
	for (int i = 0; i < sizeof(verticesKnight); i+=8)
	{
		verticesKnight[i] -= deslocation;
	}
	verticesKnight[6] = 1.0;
	verticesKnight[7] = 0.0;
	verticesKnight[14] = 0.0;
	verticesKnight[15] = 0.0;
	verticesKnight[22] = 0.0;
	verticesKnight[23] = 1.0;
	verticesKnight[30] = 0.0;
	verticesKnight[31] = 1.0;
	verticesKnight[38] = 1.0;
	verticesKnight[39] = 1.0;
	verticesKnight[46] = 1.0;
	verticesKnight[47] = 0.0;
}

int verifyCollision(GLfloat vertice, float transl) {
    if ((vertice + transl > 1.0f) || vertice + transl < -1.0f) {
        return 1;
    }
    return 0;
}

// Fun��o de callback de teclado - s� pode ter uma inst�ncia (deve ser est�tica se
// estiver dentro de uma classe) - � chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}


int setupGeometryBackground()
{
	// Aqui setamos as coordenadas x, y e z do tri�ngulo e as armazenamos de forma
	// sequencial, j� visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do v�rtice (coordenada, cores, coordenadas de textura, normal, etc)
	// Pode ser arazenado em um VBO �nico ou em VBOs separados
	GLfloat vertices[] = {
		//x     y    z    r    g    b    s    t
		//Triangulo 0
		-1.0 , -1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0,  //v0
		 1.0 , -1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0,  //v1
		 1.0 ,  1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0,   //v2 
		 //Triangulo 1
		 1.0 , 1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0,  //v3
		-1.0 , 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0,  //v4
		-1.0 , -1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, //v5 
	};


	GLuint VBO, VAO;
	//Gera��o do identificador do VBO
	glGenBuffers(1, &VBO);
	//Faz a conex�o (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//                                              vertices.data()  
	//Gera��o do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de v�rtices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);
	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localiza��o no shader * (a localiza��o dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se est� normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 

	//Atributo 0 - posi��o
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo 1 - cor
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//Atributo 2 - coordenadas de textura
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);


	// Observe que isso � permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de v�rtice 
	// atualmente vinculado - para que depois possamos desvincular com seguran�a
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (� uma boa pr�tica desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}

int setupGeometryKnight() {
	GLuint VBO, VAO;
	//Gera��o do identificador do VBO
	glGenBuffers(1, &VBO);
	//Faz a conex�o (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesKnight), verticesKnight, GL_STATIC_DRAW);
	//                                              vertices.data()  
	//Gera��o do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de v�rtices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);
	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localiza��o no shader * (a localiza��o dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se est� normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 

	//Atributo 0 - posi��o
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo 1 - cor
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//Atributo 2 - coordenadas de textura
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);


	// Observe que isso � permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de v�rtice 
	// atualmente vinculado - para que depois possamos desvincular com seguran�a
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (� uma boa pr�tica desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}

void drawScene(int VAO, int texID)
{
	glBindTexture(GL_TEXTURE_2D, texID);
	glBindVertexArray(VAO); //Conectando ao buffer de geometria
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0); //Desconectando o buffer de geometria
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

