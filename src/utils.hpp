#ifndef PLAYGROUND_UTILS_HPP
#define PLAYGROUND_UTILS_HPP

#include <glm/glm.hpp>
#include <vector>
using namespace std;

struct Triangle {
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
    vector<float> poids; // poids des os
};

struct Os {
    int ID;
    glm::vec3 Position;
    glm::mat4 world_matrix;
    glm::mat4 user_local_matrix; // transformation local de l'os défini par l'utilisateur
    glm::mat4 bind_matrix; // position et orientation de départ
    glm::mat4 bind_local_matrix; // position et orientation vis-à-vis de l'os précédent
    vector<int> fils;
    Os* pere;
};

#endif //PLAYGROUND_UTILS_HPP
