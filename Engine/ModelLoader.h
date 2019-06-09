#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <GLM/glm.hpp>

#include "ModelDataStructs.h"

#include <string>
#include <vector>

struct SDL_Surface;

namespace utilities{

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
		//parses a wavefront file(.obj) and reads data about the mesh (positions, uv coords, normals, indices)
		static renderer::MeshData loadObject(const std::string& fileName);

		//parses a grayscale map containing info about a heightfield
		static renderer::MeshData loadTerrainHeightField(const std::string& fileName, float side_size, float height_mult, int& num_rows, std::vector<float>& heights, float& min, float& max);
		
	private:
		//helper methods
		static float getHeight(int row, int col, SDL_Surface* image);
		static float getHeight(int row, int col, int num_cols, std::vector<float>& heights);
		static glm::vec3 getNormal(int x, int z, int height_mult, std::vector<float>& heights);

		static void computeAABB(const std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& aabb);

		//methods for buffering 3d mesh data
		static renderer::MeshData loadToVAO(const IndexedModel& model);

		ObjectLoader(){};

	};

}
#endif // OBJLOADER_H
