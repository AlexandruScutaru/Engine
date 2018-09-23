#ifndef RESOURCE_MANAGER
#define RESOURCE_MANAGER

#include "TexturedModel.h"

#include <vector>
#include <unordered_map>

namespace utilities{

	class ResourceManager{
	public:
		static renderer::MeshData* getMesh(const std::string& path);
		static renderer::TextureData* getTexture(const std::string& path);
		static renderer::TexturedModel* loadModel(const std::string& file);
		static renderer::TexturedModel* loadModel(int shape);
		static char* IndexToShape(int index);

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