#ifndef PLAYGROUND_UTILS_HPP
#define PLAYGROUND_UTILS_HPP

#include <glm/glm.hpp>
#include <vector>
using namespace std;

struct Triangle{
    //Indices sommets
    unsigned int S1;
    unsigned int S2;
    unsigned int S3;
    //Normal au triangle
    glm::vec3 Normal;
};


struct Vertex {
    glm::vec3 Position;
    // normal au sommet
    glm::vec3 Normal;
    glm::vec2 UVcoords;
    vector<Triangle> faces;
};

#endif //PLAYGROUND_UTILS_HPP
