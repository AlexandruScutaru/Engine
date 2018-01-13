#ifndef RESOURCE_MANAGER
#define RESOURCE_MANAGER

#include "TextureLoader.h"
#include "ModelLoader.h"
#include "TexturedModel.h"
#include <vector>
#include <map>

namespace renderer{

	class ResourceManager{
	public:
		static MeshData* getMesh(const std::string& path);
		static TextureData* getTexture(const std::string& path);

		//static unsigned int loadModel(const std::string& object_name = "default", const std::string& texture_name = "default");
		//static TexturedModel* getModelAt(unsigned int index);

		static void ClearData();

	private:
		ResourceManager(){}

		static std::map<std::string, TextureData> m_texturesMap;
		static std::map<std::string, MeshData> m_meshesMap;

		//static std::vector<TexturedModel> m_texturedModels;
	};

}

#endif // !RESOURCE_MANAGER