#ifndef LEARNOPENGL_MESH_HPP
#define LEARNOPENGL_MESH_HPP


#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shaders.hpp"
#include "utils.hpp"
#include <string>
#include <vector>

using namespace std;



class Mesh {
public:

    Mesh();
    void initMesh(vector<Vertex> vertices, vector<unsigned int> indices);
    void DrawSurface(vector<unsigned int> textures);
    void DrawControls();
    void DrawBezierCurve();
    void deleteRessources();

private:
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO;
    void setupMesh();
};

#endif
