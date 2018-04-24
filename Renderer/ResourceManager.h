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

		//static unsigned int addTexturedModel(TexturedModel& model);
		//static TexturedModel* getTexturedModelAt(unsigned int index);

		static TexturedModel* loadModel(const std::string& file);
		static TexturedModel* loadModel(int shape);
		static char* IndexToShape(int index);

		static void Init();
		static void ClearData();

		enum CollisionShapes{
			SHAPE_CUBE = 0,
			SHAPE_SPHERE,
			SHAPE_CILINDER,
			SHAPE_CONE,
			SHAPE_CAPSULE
		};

	private:
		ResourceManager(){}

		static std::map<std::string, TextureData> m_texturesMap;
		static std::map<std::string, MeshData> m_meshesMap;
		static std::map<std::string, TexturedModel> m_modelsMap;
	};

}

#endif // !RESOURCE_MANAGER