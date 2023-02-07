#ifndef LEARNOPENGL_BEZIERCURVE_HPP
#define LEARNOPENGL_BEZIERCURVE_HPP


#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shaders.hpp"
#include "mesh.hpp"
#include <string>
#include <vector>

using namespace std;

class BezierCurve
{
public:
    vector<Vertex> controlPoints;
    vector<Vertex> bezierPoints;

    BezierCurve(float nbSegments = 50.0);

    void init(vector<glm::vec3> vertices);
    void Draw();
    void deCasteljau(float t);
    void bezier();
    void del();

private:
    vector<unsigned int> controlIndices;
    vector<unsigned int> bezierIndices;
    int nbControles;
    float tailleSegments;
    Mesh controlMesh;
    Mesh curveMesh;
};


#endif
