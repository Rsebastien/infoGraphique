#include "skinning.hpp"

Skinning::Skinning(BezierSurface bezierSurface){
    this->rest_pose_vertices = bezierSurface.getSurfaceVertices();
    this->vertices = bezierSurface.getSurfaceVertices();
    this->lignes = bezierSurface.getLignes();
    this->colonnes = bezierSurface.getColonnes();
    this->indices = bezierSurface.getSurfaceIndices();
    this->surfaceMesh.initMesh(rest_pose_vertices, indices);
    nb_bones = 2;
    glm::mat4 identity;
    for(int i=0; i<nb_bones; i++){
        skinning_transformations.push_back(identity);
    }
    createBones();
    initWeights();
}

// Création de 3 os pour une surface de bézier
void Skinning::createBones() {
    Os premier, second;
    premier.ID = 0;
    premier.pere = nullptr;
    premier.Position = rest_pose_vertices[(lignes/2+1) * colonnes].Position;
    vertices[(lignes/2+1) * colonnes].Normal = glm::vec3(0.);
//    premier.world_matrix = glm::translate(glm::mat4(1.0), premier.Position);
    premier.world_matrix = glm::mat4(1.0);
    premier.user_local_matrix = glm::mat4(1.0);
//    premier.user_local_matrix = glm::rotate(premier.world_matrix, glm::radians(0.2), glm::vec3(0., 1., 0.));
    premier.bind_local_matrix = glm::translate(glm::mat4(1.0), premier.Position);
    premier.bind_matrix = premier.bind_local_matrix;

    second.ID = 1;
    second.pere = &premier;
    second.Position = rest_pose_vertices[(lignes/2+1) * colonnes + colonnes/2].Position;
    vertices[(lignes/2+1) * colonnes + colonnes/2].Normal = glm::vec3(0.);
//    second.world_matrix = glm::translate(glm::mat4(1.0), second.Position);
    second.world_matrix = glm::mat4(1.0);
    second.user_local_matrix = glm::mat4(1.0);
    second.bind_matrix = glm::translate(glm::mat4(1.0), second.Position);
    second.bind_local_matrix = glm::inverse(second.pere->bind_matrix) * second.bind_matrix;

    premier.fils.push_back(second.ID);

    bones.push_back(premier);
    bones.push_back(second);
}

void Skinning::initWeights() {

    for(int i=0; i<lignes; i++){
        int count = 0;
        for(int j=0; j<colonnes; j++){
            if(j%10 == 0){
                count++;
            }
            if(count <= 2){
                vertices[i*colonnes+j].poids.push_back(1.);
                vertices[i*colonnes+j].poids.push_back(0.);
            }else if(count == 3) {
                float poids = j % 10 * 0.1;
                vertices[i * colonnes + j].poids.push_back(1. - poids);
                vertices[i * colonnes + j].poids.push_back(poids);
            }else{
                vertices[i*colonnes+j].poids.push_back(0.);
                vertices[i*colonnes+j].poids.push_back(1.);
            }
        }
    }
}

// Initialisation des poids pour chaque sommets en fonction de la distance euclidienne entre le sommet et les os
// Les poids sont normalisés et inversés (le plus proche a plus de poids)
void Skinning::initWeights_euclidean() {
    for(int i=0; i<lignes; i++){
        for(int j=0; j<colonnes; j++){
            vector<float> tmp;
            glm::vec3 pos_vertex = vertices[i*colonnes+j].Position;

            for(int k=0; k<bones.size(); k++){
                float dist_to_bone = sqrt(pow(pos_vertex[0] - bones[k].Position[0], 2) +
                                       pow(pos_vertex[1] - bones[k].Position[1], 2) +
                                       pow(pos_vertex[2] - bones[k].Position[2], 2));
                tmp.push_back(dist_to_bone);
            }

            for(int k=0; k<tmp.size(); k++){
                auto sum = reduce(tmp.begin(), tmp.end());
                float tmp_norm = 1.f - (tmp[k] / sum);
                vertices[i*colonnes+j].poids.push_back(tmp_norm);
            }
        }
    }
}

// rotation/translation des os, ici une simple rotation du premier os pour voir si tout fonctionne
void Skinning::updateBones(float angle, glm::vec3 axes) {
    // A FAIRE : déplacement des os

//    bones[0].world_matrix = glm::rotate(bones[0].world_matrix, (float)glfwGetTime(), glm::vec3(angle));
//    bones[1].world_matrix = glm::rotate(bones[1].world_matrix, glm::radians(angle), glm::vec3(glm::vec3(0., 0., 1.)));
    bones[1].user_local_matrix = glm::rotate(bones[1].user_local_matrix, glm::radians(angle), axes);

    rec_skinning_transformations(bones[0].ID, glm::mat4(1.));
//    for(const Os& bone : bones){
//        skinning_transformations[bone.ID] = bone.world_matrix * glm::inverse(bone.bind_matrix);
//    }

    updateVertices();
}


void Skinning::rec_skinning_transformations(int id, glm::mat4 W_parent){
    // W_j = W_p(j) * L_j * Ul_j
    bones[id].world_matrix = W_parent * bones[id].bind_local_matrix * bones[id].user_local_matrix;
    // T_j = world_matrix(j) * inv(bind_matrix(j))
    skinning_transformations[id] = bones[id].world_matrix * glm::inverse(bones[id].bind_matrix);
    for(int i : bones[id].fils){
        rec_skinning_transformations(i, bones[id].world_matrix);
    }
}


void Skinning::updateVertices() {
    // A FAIRE : rotation et translation des points du maillage en fonction des poids et
    // des translations + rotations des os

    for(int i=0; i<vertices.size(); i++){
        glm::mat4 sum_T(1.0f);
        for(const Os& bone : bones){
            // sum += poids(j) * T_j
            sum_T +=  vertices[i].poids[bone.ID] * skinning_transformations[bone.ID];
        }

        glm::vec4 new_pos = sum_T * glm::vec4(rest_pose_vertices[i].Position, 1.f); //
        new_pos = new_pos/new_pos[3];
        vertices[i].Position = glm::vec3(new_pos);
    }

//    surfaceMesh.initMesh(vertices, indices);
    surfaceMesh.updateVBO(vertices);
}


void Skinning::Draw(vector<unsigned int> textures){
    surfaceMesh.DrawSurface(textures);
}