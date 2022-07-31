#include "MaterialManager.h"
#include "Material.h"

MaterialManager::~MaterialManager()
{
	for (int i = 0; i < m_Materials.size(); i++)
	{
		delete m_Materials[i];
		m_Materials[i] = nullptr;
	}
}

Material* MaterialManager::AddMaterial(Material* material)
{
	m_Materials.push_back(material);
	return material;
}
