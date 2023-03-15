#ifndef PLAYGROUND_SKINNING_HPP
#define PLAYGROUND_SKINNING_HPP

#include "utils.hpp"
#include "mesh.hpp"
#include "bezierSurface.hpp"

#include <GLFW/glfw3.h>
#include <bits/stdc++.h>
#include <math.h>
#include <string>
#include <vector>

using namespace std;

class Skinning {
public:

    Skinning(BezierSurface bezierSurface);

    void Draw(vector<unsigned int> textures);
    void updateVertices();
    void updateBones(float angle);
    void initWeights_euclidean();
    void initWeights();
    void createBones();
    void rec_skinning_transformations(int id, glm::mat4 W_parent);

private:
    vector<glm::mat4> skinning_transformations;
    vector<Vertex> rest_pose_vertices;
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    Mesh surfaceMesh;
    vector<Os> bones;
    unsigned int nb_bones;
    unsigned int lignes = 0;
    unsigned int colonnes = 0;
};

#endif //PLAYGROUND_SKINNING_HPP
