#include "Geometry.h"

Geometry::Geometry(const Elite::RGBColor& color, Material* material)
	: m_Color{ color }
	, m_pMaterial{ material }
{
}

Geometry::~Geometry()
{

}
