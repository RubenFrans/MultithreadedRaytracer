#include "TriangleMesh.h"
#include "Triangle.h"
#include "Material_Lambert.h"

TriangleMesh::TriangleMesh(Elite::RGBColor color, Material* material, Elite::FVector3& position, std::vector<int>& idList, std::vector<Elite::FPoint3>& vertexbuffer)
    : Geometry{ color, material }
    , m_Position{ position }
    , m_IdList{ idList }
    , m_VertexBuffer{ vertexbuffer }
{
    AssembleTriangles();
}

TriangleMesh::~TriangleMesh()
{
    for (int i = 0; i < m_Triangles.size(); i++)
    {
        delete m_Triangles[i];
        m_Triangles[i] = nullptr;
    }
}

bool TriangleMesh::Hit(const Ray& ray, HitRecord& hit) const
{
    // tested this works
    for (Triangle* t : m_Triangles) {

        if (t->Hit(ray, hit))
            return true;
    }

    return false;
}

void TriangleMesh::Rotate(float deltaT) {

    for (Triangle* t : m_Triangles) {

       t->Rotate(deltaT);
    }

}

void TriangleMesh::AssembleTriangles()
{
    for (size_t i = 0; i + 3 <= m_IdList.size(); i+=3)
    {
        m_Triangles.push_back(new Triangle{m_Color, m_pMaterial, m_Position,
            m_VertexBuffer[m_IdList[i]]
            , m_VertexBuffer[m_IdList[i + 1]]
            , m_VertexBuffer[m_IdList[i + 2]]
            , CullMode::backface});
    }
}
