#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <SDL/SDL.h>
#include <GLM/glm.hpp>

#include <string>
#include <vector>

#include "ModelDataStructs.h"

namespace renderer{

	struct OBJIndex
	{
		unsigned int vertexIndex;
		unsigned int uvIndex;
		unsigned int normalIndex;

		bool operator<(const OBJIndex& r) const { return vertexIndex < r.vertexIndex; }
	};

	class IndexedModel
	{
	public:
		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> texCoords;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;

		void CalcNormals();
	};

	class OBJModel
	{
	public:
		std::vector<OBJIndex> OBJIndices;
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;
		bool hasUVs;
		bool hasNormals;

		OBJModel(const std::string& fileName);

		IndexedModel ToIndexedModel();
	private:
		unsigned int FindLastVertexIndex(const std::vector<OBJIndex*>& indexLookup, const OBJIndex* currentIndex, const IndexedModel& result);
		void CreateOBJFace(const std::string& line);

		glm::vec2 ParseOBJVec2(const std::string& line);
		glm::vec3 ParseOBJVec3(const std::string& line);
		OBJIndex ParseOBJIndex(const std::string& token, bool* hasUVs, bool* hasNormals);
	};

	//static class used to load various 3d meshes into memory
	class ObjectLoader{
	public:
		//parses a wavefront file(.obj) and reads data about the mesh
		//positions, uv coords, normals, indices
		static MeshData loadObject(const std::string& fileName);
		//loads a cube
		static MeshData loadCube(std::vector<glm::vec3>& positions, int numVertices);
		
	private:
		//methods for buffering 3d mesh data
		static MeshData loadToVAO(const IndexedModel& model);
		static MeshData loadToVAO(std::vector<glm::vec3>& positions,
								  std::vector<glm::vec2>& texCoords,
								  std::vector<glm::vec3>& normals,
								  unsigned int numVertices,
								  unsigned int* indices,
								  unsigned int numIndices);

		ObjectLoader(){};

	};

}
#endif // OBJLOADER_H
