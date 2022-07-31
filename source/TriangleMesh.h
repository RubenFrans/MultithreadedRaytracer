#pragma once
#include "Geometry.h"
#include "EMath.h"
#include <vector>

class Triangle;

class TriangleMesh :
    public Geometry
{
public:
    TriangleMesh(Elite::RGBColor color, Material* material, Elite::FVector3& position, std::vector<int>& idList, std::vector<Elite::FPoint3>& vertexbuffer);
    ~TriangleMesh();
    TriangleMesh(const TriangleMesh& other) = delete; // Copy constructor
    TriangleMesh(TriangleMesh&& other) = delete; // move constructor
    TriangleMesh& operator=(const TriangleMesh& other) = delete; // copy assignment
    TriangleMesh& operator=(TriangleMesh&& other) = delete; // Move assignment
    virtual bool Hit(const Ray& ray, HitRecord& hit) const override;
    void Rotate(float deltaT);
private:
    Elite::FVector3 m_Position;
    std::vector<int> m_IdList;
    std::vector<Elite::FPoint3> m_VertexBuffer;
    std::vector<Triangle*> m_Triangles;
    void AssembleTriangles();
};

