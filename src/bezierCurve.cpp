
#include "bezierCurve.hpp"


BezierCurve::BezierCurve(float nbSegments){
    this->tailleSegments = 1/nbSegments;
}


void BezierCurve::init(vector<glm::vec3> vertices){
    nbControles = vertices.size();
    for(int i=0; i<vertices.size(); i++){
        Vertex vertex;
        vertex.Position = vertices[i];
        controlPoints.push_back(vertex);
    }
    bezier();
    curveMesh.initMesh(bezierPoints, bezierIndices);

    for(int i=0; i<nbControles; i++){
        controlIndices.push_back(i);
    }
    controlMesh.initMesh(controlPoints, controlIndices);
}


void BezierCurve::Draw(){
    controlMesh.DrawControls();
    curveMesh.DrawBezierCurve();
}

void BezierCurve::deCasteljau(float t) {

    vector<Vertex> temp = controlPoints;
    for (int i = 1; i < controlPoints.size(); i++)
        for (int j = 0; j < controlPoints.size() - i; j++)
            temp[j].Position = (temp[j].Position * (1 - t)) + (temp[j + 1].Position * t);

    if (!temp.empty()){
        bezierPoints.push_back(temp[0]);
        bezierIndices.push_back(static_cast<unsigned int>(bezierPoints.size()-1));
    }
}

void BezierCurve::bezier() {
//        bezierPoints.clear();
    for (float t = 0; t < 1; t += tailleSegments)
        deCasteljau(t);

//        for(Vertex ver : bezierPoints){
//            cout << ver.Position[0] << "," << ver.Position[1] << "," << ver.Position[2]<< endl;
//        }
}

void BezierCurve::del(){
    controlMesh.deleteRessources();
    curveMesh.deleteRessources();
}