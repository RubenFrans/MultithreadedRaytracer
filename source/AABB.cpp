#include "AABB.h"
#include <algorithm>

bool AABB::Intersect(const Ray& ray) const
{
    float tmin = std::numeric_limits<float>::lowest();
    float tmax = std::numeric_limits<float>::max();

    for (int i = 0; i < 3; i++)
    {
        if (fabs(ray.GetDirection()[i]) < 1e-6f)
        {
            if (ray.GetOrigin()[i] < m_MinExtent[i] || ray.GetOrigin()[i] > m_MaxExtent[i])
            {
                return false;
            }
        }
        else
        {
            float ood = 1.0f / ray.GetDirection()[i];
            float t1 = (m_MinExtent[i] - ray.GetOrigin()[i]) * ood;
            float t2 = (m_MaxExtent[i] - ray.GetOrigin()[i]) * ood;
            if (t1 > t2)
            {
                std::swap(t1, t2);
            }
            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);
            if (tmin > tmax)
            {
                return false;
            }
        }
    }

    return true;
}
