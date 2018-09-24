#include "ResourceManager.h"
#include "TextureLoader.h"
#include "ModelLoader.h"
#include "CollisionBody.h"

#include <fstream>

#include <JSON/json.hpp>
using json = nlohmann::json;
using namespace renderer;

namespace utilities{

	std::unordered_map<std::string, renderer::TextureData> ResourceManager::m_texturesMap;
	std::unordered_map<std::string, renderer::MeshData> ResourceManager::m_meshesMap;
	std::unordered_map<std::string, renderer::TexturedModel> ResourceManager::m_modelsMap;


	renderer::MeshData* ResourceManager::getMesh(const std::string& path){
		auto it = m_meshesMap.find(path); //auto looks for the return type and automatically assigns it
											//here auto is std::map<std::string, GLTexture>::iterator
		if(it == m_meshesMap.end()){
			//Load the OBJ
			renderer::MeshData mesh = ObjectLoader::loadObject(path);

			//m_meshesMap.push_back(mesh);
			auto ret = m_meshesMap.insert(make_pair(path, mesh));

			return &(ret.first->second);
		}
		return &(it->second);
	}

	renderer::TextureData* ResourceManager::getTexture(const std::string& path){
		//lookup the texture and see if its in the map
		auto it = m_texturesMap.find(path); //auto looks for the return type and automatically assigns it
											//here auto is std::map<std::string, GLTexture>::iterator
		if(it == m_texturesMap.end()){
			//Load the texture
			renderer::TextureData newTexture = TextureLoader::loadTexture(path);

			auto ret = m_texturesMap.insert(make_pair(path, newTexture));

			return &(ret.first->second);
		}
		return &(it->second);
	}

	renderer::TexturedModel* ResourceManager::loadModel(const std::string& file){
		std::string path = "res/gameobjects/" + file;
		auto it = m_modelsMap.find(path);
		if(file == "default"){
			m_modelsMap.erase(path);
			it = m_modelsMap.end();
		}
		if(it == m_modelsMap.end()){
			std::ifstream in(path);
			json obj;
			in >> obj;
			in.close();

			renderer::Material mat(getTexture("res/textures/" + obj["diff"].get<std::string>()),
						           getTexture("res/textures/" + obj["spec"].get<std::string>()));
			renderer::MeshData* mesh = getMesh("res/models/" + obj["mesh"].get<std::string>());
			bool billboard = obj["billboard"].get<bool>();

			renderer::TexturedModel model = renderer::TexturedModel(mesh, mat, billboard);

			auto ret = m_modelsMap.insert(make_pair(path, model));
			return &(ret.first->second);
		}

		return &(it->second);
	}

	renderer::TexturedModel * ResourceManager::loadModel(int shape){
		return loadModel(IndexToShape(shape));
	}

	void ResourceManager::Init(){}

	void ResourceManager::ClearData(){
		for(auto mesh : m_meshesMap){
			glDeleteVertexArrays(1, &(mesh.second.vertexArrayObject));
			for(int i = 0; i < renderer::NUM_BUFFERS; i++){
				glDeleteBuffers(1, &(mesh.second.vertexArrayBuffers[i]));
			}
		}
		m_meshesMap.clear();

		for(auto textureData : m_texturesMap){
			glDeleteTextures(1, &(textureData.second.id));
		}
		m_texturesMap.clear();
		m_modelsMap.clear();
	}

	
	const char *ResourceManager::IndexToShape(int index) {
		switch (index) {
		case CollisionShapes::SHAPE_CUBE:
			return "cube";
		case CollisionShapes::SHAPE_SPHERE:
			return "sphere";
		case CollisionShapes::SHAPE_CILINDER:
			return "cilinder";
		case CollisionShapes::SHAPE_CONE:
			return "cone";
		case CollisionShapes::SHAPE_CAPSULE:
			return "capsule";
		}
		return "";
	}

}