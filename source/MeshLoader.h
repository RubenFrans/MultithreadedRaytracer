#pragma once
#include <vector>
#include "EMath.h"
#include <string>
class MeshLoader
{
public:
	static bool LoadMesh(std::vector<int>& idList, std::vector<Elite::FPoint3>& vertexbuffer, const std::string& filesname);
private:

};

