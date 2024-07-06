#include "Tile.h"
#include <vector>
#include <string>
#include "Shader.h"
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

class Tilemap {
public:
    Tilemap(string texturePath, string configPath, Shader* shader, vec3 viewportCenter);
    ~Tilemap();

    void draw();
    void selectTile();

    void moveSelectionNorth();
    void moveSelectionNortheast();
    void moveSelectionNorthwest();
    void moveSelectionSouth();
    void moveSelectionSoutheast();
    void moveSelectionSouthwest();
    void moveSelectionEast();
    void moveSelectionWest();
    bool canMoveTo(vec3 position);

    vec3 getSelectedTile();

private:
    void loadConfig(string configPath);
    GLuint loadTexture(string texturePath);

    GLuint textureID;
    int tilesX, tilesY, texTilesX, texTilesY;
    float tileWidth, tileHeight;
    vector<Tile*> tiles;
    Shader* shader;
    vec3 viewportCenter;
    vec3 selectedTile;
};