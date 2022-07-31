#include "MeshLoader.h"
#include <fstream>
#include "MeshNotLoadedException.h"
#include <sstream>
#include <iostream>
bool MeshLoader::LoadMesh(std::vector<int>& idList, std::vector<Elite::FPoint3>& vertexbuffer, const std::string& filename)
{
    std::ifstream in;
    try {
        in = std::ifstream{filename, std::ios::in};

        if (!in)
            throw MeshNotLoadedException();

    }
    catch (MeshNotLoadedException ex) {
        std::cout << "Could not find " << filename << " are you sure it is in the source folder?" << std::endl;
        return false;
    }

    std::string line;

    while (std::getline(in, line))
    {
        if (line.substr(0, 2) == "v ") {

            std::istringstream v(line.substr(2));

            float x, y, z;
            v >> x;
            v >> y;
            v >> z;
            vertexbuffer.push_back(Elite::FPoint3{ x, y , z });
        }
        else if(line.substr(0, 2) == "f "){


            std::istringstream v(line.substr(2));

            int a, b, c;
            v >> a;
            v >> b;
            v >> c;
           
            idList.push_back(a-1);
            idList.push_back(b-1);
            idList.push_back(c-1);

        }
    }

    return true;
}
