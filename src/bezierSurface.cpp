#include "bezierSurface.hpp"

#include <utility>
#include "bezierCurve.hpp"



BezierSurface::BezierSurface(vector<vector<glm::vec3>> controlPoints){
    this->controlPoints = std::move(controlPoints);


    for(int u=0; u<100; u+=pas_u){
        this->lignes++;
        for(int v=0; v<100; v+=pas_v){
            if(u==0)
                this->colonnes++;

            float up = (float)u/100.f;
            float vp = (float)v/100.f;

            Vertex vertex;
            vertex.Position = evaluateBezierSurface(up, vp);
            vertex.Normal = glm::vec3(0.0f);
            vertex.UVcoords = glm::vec2(up, vp);
            bezierSurfacePoints.push_back(vertex);
        }
    }

    for(unsigned int i=0; i<this->lignes-1; i++){
        for(unsigned int j=0; j<this->colonnes-1; j++){
            bezierSurfaceIndices.push_back(i*colonnes+j);
            bezierSurfaceIndices.push_back(i*colonnes+j+1);
            bezierSurfaceIndices.push_back((i+1)*colonnes+j+1);
            bezierSurfaceIndices.push_back(i*colonnes+j);
            bezierSurfaceIndices.push_back((i+1)*colonnes+j+1);
            bezierSurfaceIndices.push_back((i+1)*colonnes+j);
        }
    }

    CalculateNormals();
    surfaceMesh.initMesh(bezierSurfacePoints, bezierSurfaceIndices);
}

void BezierSurface::CalculateNormals(){
    vector<Triangle> faces;

    //On calcule chaque normal de chaque face
    for (int i=0; i<bezierSurfaceIndices.size(); i+=3){

        Triangle face{};
        face.S1 = bezierSurfaceIndices[i];
        face.S2 = bezierSurfaceIndices[i+1];
        face.S3 = bezierSurfaceIndices[i+2];

        glm::vec3 u = glm::normalize(bezierSurfacePoints[face.S1].Position-bezierSurfacePoints[face.S2].Position);
        glm::vec3 v = glm::normalize(bezierSurfacePoints[face.S3].Position-bezierSurfacePoints[face.S2].Position);
        face.Normal = glm::cross(u,v);

        //On ajoute la face
        faces.push_back(face);

        //On ajoute aux sommets concernés la face dans laquelle ils interviennent
        bezierSurfacePoints[face.S1].faces.push_back(face);
        bezierSurfacePoints[face.S2].faces.push_back(face);
        bezierSurfacePoints[face.S3].faces.push_back(face);
    }

    //On calcule les normales de chaque sommet
    for (auto & bezierSurfacePoint : bezierSurfacePoints){

        auto normal = glm::vec3(0.0f);

        //On somme les normales de chaque face dans lesquelles le sommet intervient
        int x = bezierSurfacePoint.faces.size();
        if(x%2==1 && x != 1){
            x -= 1;
        }
        for (int j=0; j<x;j++){
            normal+=bezierSurfacePoint.faces[j].Normal;
        }
        bezierSurfacePoint.Normal = glm::normalize(normal);
    }
}

//Calculer le point P(U,V)
glm::vec3 BezierSurface::evaluateBezierSurface(float u, float v){

    vector<glm::vec3> points;
    for (unsigned int i=0; i<4; i++){
        points.push_back(deCasteljau(v,controlPoints[i]));
    }

    glm::vec3 puv = deCasteljau(u,points);

    return puv;
}

//Algorithme de Casteljau pour un t et une liste de points de controle donnés
glm::vec3 BezierSurface::deCasteljau(float t, vector<glm::vec3> pointsControl){
    vector<glm::vec3> elem;

    //Initialisation des premiers éléments
    for (int i=0; i<pointsControl.size(); i++){
        elem.push_back(pointsControl[i]);
    }

    //Itération
    for (int r=1; r<pointsControl.size(); r++){
        for (int i=0; i<pointsControl.size()-r; i++){
            //A chaque itération on remplace l'élément i par l'interpolation des anciens éléments i et i+1
            elem[i] = (1 - t) * elem[i] + t * elem[i + 1];
        }
    }

    //On renvoie la position de notre point
    return elem[0];
}

void BezierSurface::Draw(vector<unsigned int> textures){
    surfaceMesh.DrawSurface(textures);
}


void BezierSurface::del(){
    surfaceMesh.deleteRessources();
}

vector<Vertex> BezierSurface::getSurfaceVertices(){
    return this->bezierSurfacePoints;
}

vector<unsigned int> BezierSurface::getSurfaceIndices(){
    return this->bezierSurfaceIndices;
}

Mesh BezierSurface::getSurfaceMesh(){
    return this->surfaceMesh;
}

unsigned int BezierSurface::getLignes(){
    return this->lignes;
}

unsigned int BezierSurface::getColonnes(){
    return this->colonnes;
}

