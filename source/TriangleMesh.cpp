#include "TriangleMesh.h"
#include "Triangle.h"
#include "Material_Lambert.h"
#include <algorithm>

TriangleMesh::TriangleMesh(Elite::RGBColor color, Material* material, Elite::FVector3& position, std::vector<int>& idList, std::vector<Elite::FPoint3>& vertexbuffer)
    : Geometry{ color, material }
    , m_Position{ position }
    , m_IdList{ idList }
    , m_VertexBuffer{ vertexbuffer }
    , m_BoundingBox{}
{
    AssembleTriangles();
    GenerateBoundingBox();
}

TriangleMesh::~TriangleMesh()
{
    for (int i = 0; i < m_Triangles.size(); i++)
    {
        delete m_Triangles[i];
        m_Triangles[i] = nullptr;
    }
}

void TriangleMesh::GenerateBoundingBox()
{
    Elite::FVector3 min, max;
    min.x = min.y = min.z = std::numeric_limits<float>::max();
    max.x = max.y = max.z = -std::numeric_limits<float>::max();

    for (Triangle* triangle : m_Triangles)
    {
        min.x = std::min(min.x, std::min(triangle->GetV0().x, std::min(triangle->GetV1().x, triangle->GetV2().x)));
        min.y = std::min(min.y, std::min(triangle->GetV0().y, std::min(triangle->GetV1().y, triangle->GetV2().y)));
        min.z = std::min(min.z, std::min(triangle->GetV0().z, std::min(triangle->GetV1().z, triangle->GetV2().z)));

        max.x = std::max(min.x, std::max(triangle->GetV0().x, std::min(triangle->GetV1().x, triangle->GetV2().x)));
        max.y = std::max(min.y, std::max(triangle->GetV0().y, std::min(triangle->GetV1().y, triangle->GetV2().y)));
        max.z = std::max(min.z, std::max(triangle->GetV0().z, std::min(triangle->GetV1().z, triangle->GetV2().z)));
    }

    m_BoundingBox = AABB{ min * 1.5, max * 1.5};
}

bool TriangleMesh::Hit(const Ray& ray, HitRecord& hit) const
{
    if (!m_BoundingBox.Intersect(ray))
    {
        return false;
    }

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

    GenerateBoundingBox();
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
