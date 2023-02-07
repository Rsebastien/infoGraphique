#ifndef LEARNOPENGL_BEZIERSURFACE_HPP
#define LEARNOPENGL_BEZIERSURFACE_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shaders.hpp"
#include "mesh.hpp"
#include "bezierCurve.hpp"

#include <string>
#include <vector>
using namespace std;


class BezierSurface
{
public:

//    BezierSurface();
    BezierSurface(vector<vector<glm::vec3>> controlPoints);
//    void init(BezierCurve bezierCurve1, BezierCurve bezierCurve2);
    void CalculateNormals();
    void del();
    void Draw();
    glm::vec3 evaluateBezierSurface(float u, float v);
    glm::vec3 deCasteljau(float t, vector<glm::vec3> points);

private:
    vector<Vertex> bezierSurfacePoints;
    vector<unsigned int> bezierSurfaceIndices;
    vector<vector<glm::vec3>> controlPoints;
    int pas_u = 2;
    int pas_v = 2;
    Mesh surfaceMesh;
};

#endif


