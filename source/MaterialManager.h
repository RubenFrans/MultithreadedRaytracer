#pragma once
#include <vector>

class Material;

class MaterialManager
{

public:
	MaterialManager() = default;
	~MaterialManager();

	Material* AddMaterial(Material* material);

private:

	std::vector<Material*> m_Materials;

};

