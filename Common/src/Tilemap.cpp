#include "Tilemap.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include<stb_image.h>
#include "Shader.h"

using namespace glm;

Tilemap::Tilemap(string texturePath, string configPath, Shader* shader, vec3 viewportCenter, vector<GLuint> itens):
 shader(shader), viewportCenter(viewportCenter), selectedTile(viewportCenter), itens(itens) {
    textureID = loadTexture(texturePath);
    loadConfig(configPath);
}

Tilemap::~Tilemap() {
    for (auto tile : tiles) {
        delete tile;
    }
}

void Tilemap::loadConfig(string configPath) {
    ifstream file(configPath);
    if (!file.is_open()) {
        throw runtime_error("Could not open config file");
    }

    string line;
    getline(file, line);
    istringstream iss(line);
    iss >> texTilesX >> texTilesY;

    float tileSizeX = static_cast<float>(1) / texTilesX;
    float tileSizeY = static_cast<float>(1) / texTilesY;

    getline(file, line);
    iss.clear();
    iss.str(line);
    iss >> tileWidth >> tileHeight;

    int itenWidth, itenHeight;

    getline(file, line);
    iss.clear();
    iss.str(line);
    iss >> itenWidth >> itenHeight;

    getline(file, line);
    iss.clear();
    iss.str(line);
    iss >> tilesX >> tilesY;

    float initialPosX = 0.0f;
    float initialPosY = 768.0f;

    int x = 0;
    int y = 0;
    while (getline(file, line)) {
        iss.clear();
        iss.str(line);
        int posX, posY, intCanSelect, intHasIten, intItenTex;
        iss >> posX >> posY >> intCanSelect >> intHasIten >> intItenTex;;

        float texPosX = tileSizeX * posX;
        float texPosY = tileSizeY * posY;

        float tilePosX = initialPosX + (x * tileWidth);
        float tilePosY = initialPosY - (y * (tileHeight / 2.0f));

        // Deslocamento para linhas ímpares
        if (y % 2 != 0) {
            tilePosX += tileWidth / 2.0f;
        }

        vec3 tilePos(tilePosX, tilePosY, viewportCenter.z);
        vec3 scale(tileWidth, tileHeight, 0.0f);
        float ang = 0.0f;

        bool canSelect = false;
        bool hasIten = false;

        if (intCanSelect == 0) {
            canSelect = true;
        }
        
        if (intHasIten == 1) {
            hasIten = true;
        }

        tiles.push_back(
            new Tile(textureID, texPosX, texPosY,
             tileSizeX, tileSizeY, tilePos,
              scale, shader, ang, canSelect,
               hasIten, itens[intItenTex], 
               vec3(static_cast<float>(itenWidth), static_cast<float>(itenHeight), 0.0f)));

        if (++x >= tilesX) {
            x = 0;
            y++;
        }
    }

    file.close();
}

void Tilemap::draw() {
    selectTile();
    for (auto tile : tiles) {
        tile->draw();
    }
}

void Tilemap::moveSelectionNortheast() {
    if (canMoveTo(vec3(selectedTile.x + tileWidth / 2.0f, selectedTile.y - tileHeight / 2.0f, selectedTile.z))) {
        selectedTile.y -= tileHeight / 2.0f;
        selectedTile.x += tileWidth / 2.0f;
    }
}

void Tilemap::moveSelectionNorthwest() {
    if (canMoveTo(vec3(selectedTile.x - tileWidth / 2.0f, selectedTile.y - tileHeight / 2.0f, selectedTile.z))) {
        selectedTile.y -= tileHeight / 2.0f;
        selectedTile.x -= tileWidth / 2.0f;
    }
}

void Tilemap::moveSelectionNorth() {
    if (canMoveTo(vec3(selectedTile.x, selectedTile.y - tileHeight, selectedTile.z))) {
        selectedTile.y -= tileHeight;
    }
}

void Tilemap::moveSelectionSoutheast() {
    if (canMoveTo(vec3(selectedTile.x + tileWidth / 2.0f, selectedTile.y + tileHeight / 2.0f, selectedTile.z))) {
        selectedTile.y += tileHeight / 2.0f;
        selectedTile.x += tileWidth / 2.0f;
    }
}

void Tilemap::moveSelectionSouthwest() {
    if (canMoveTo(vec3(selectedTile.x - tileWidth / 2.0f, selectedTile.y + tileHeight / 2.0f, selectedTile.z))) {
        selectedTile.y += tileHeight / 2.0f;
        selectedTile.x -= tileWidth / 2.0f;
    }
}

void Tilemap::moveSelectionSouth() {
    if (canMoveTo(vec3(selectedTile.x, selectedTile.y + tileHeight, selectedTile.z))) {
        selectedTile.y += tileHeight;
    }
}

void Tilemap::moveSelectionEast() {
    if (canMoveTo(vec3(selectedTile.x + tileWidth, selectedTile.y, selectedTile.z))) {
        selectedTile.x += tileWidth;
    }
}

void Tilemap::moveSelectionWest() {
    if (canMoveTo(vec3(selectedTile.x - tileWidth, selectedTile.y, selectedTile.z))) {
        selectedTile.x -= tileWidth;
    }
}

vec3 Tilemap::getSelectedTile() {
    return this->selectedTile;
}

bool Tilemap::canMoveTo(vec3 position) {
    Tile* closestTile = nullptr;
    float closestDistance = numeric_limits<float>::max();
    
    for (auto tile : tiles) {
        float distance = glm::distance(tile->getPosition(), position);
        if (distance < closestDistance) {
            closestDistance = distance;
            closestTile = tile;
        }
    }

    if (closestTile) {
        return closestTile->canSelect();
    }
    return false;
}

void Tilemap::selectTile() {
    Tile* closestTile = nullptr;
    float closestDistance = numeric_limits<float>::max();

    for (auto tile : tiles) {
        float distance = glm::distance(tile->getPosition(), selectedTile);
        if (distance < closestDistance) {
            closestDistance = distance;
            closestTile = tile;
        }
    }

    if (closestTile) {
        for (auto tile : tiles) {
            tile->setSelected(false);
        }
        closestTile->setSelected(true);
    }
}

GLuint Tilemap::loadTexture(string texturePath)
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
