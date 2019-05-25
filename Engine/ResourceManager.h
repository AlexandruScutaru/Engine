#ifndef RESOURCE_MANAGER
#define RESOURCE_MANAGER

#include "TexturedModel.h"

#include <vector>
#include <unordered_map>

namespace utilities{

	class ResourceManager{
	public:
		static renderer::MeshData* getMesh(const std::string& path);
		static renderer::MeshData* genTerrainHightField(const std::string& heightFieldPath, float side_size, float height_mult, int& num_rows, std::vector<float>& heights, float& min, float& max);
		static renderer::TextureData* getTexture(const std::string& path, bool flip = true);
		static GLuint getCubemapTexture(std::vector<std::string>& paths);
		static renderer::TexturedModel* loadModel(const std::string& file);
		static renderer::TexturedModel* loadModel(int shape);
		static const char* IndexToShape(int index);

		static void Init();
		static void ClearData();

	private:
		ResourceManager(){}

		static std::unordered_map<std::string, renderer::TextureData> m_texturesMap;
		static std::unordered_map<std::string, renderer::MeshData> m_meshesMap;
		static std::unordered_map<std::string, renderer::TexturedModel> m_modelsMap;

	};

}

#endif // !RESOURCE_MANAGER