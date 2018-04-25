#include "ResourceManager.h"
#include <fstream>

#include <JSON/json.hpp>
using json = nlohmann::json;

namespace renderer{

	std::map<std::string, TextureData> ResourceManager::m_texturesMap;
	std::map<std::string, MeshData> ResourceManager::m_meshesMap;
	std::map<std::string, TexturedModel> ResourceManager::m_modelsMap;


	MeshData* ResourceManager::getMesh(const std::string& path){
		auto it = m_meshesMap.find(path); //auto looks for the return type and automatically assigns it
										  //here auto is std::map<std::string, GLTexture>::iterator
		if(it == m_meshesMap.end()){
			//Load the OBJ
			MeshData mesh = ObjectLoader::loadObject(path);

			//m_meshesMap.push_back(mesh);
			auto ret = m_meshesMap.insert(make_pair(path, mesh));

			return &(ret.first->second);
		}
		return &(it->second);
	}

	TextureData* ResourceManager::getTexture(const std::string& path){
		//lookup the texture and see if its in the map
		auto it = m_texturesMap.find(path); //auto looks for the return type and automatically assigns it
											//here auto is std::map<std::string, GLTexture>::iterator
		if(it == m_texturesMap.end()){
			//Load the texture
			TextureData newTexture = TextureLoader::loadTexture(path);

			auto ret = m_texturesMap.insert(make_pair(path, newTexture));

			return &(ret.first->second);
		}
		return &(it->second);
	}

	TexturedModel* ResourceManager::loadModel(const std::string& file){
		std::string path = "res/gameobjects/" + file;
		auto it = m_modelsMap.find(path);
		if(it == m_modelsMap.end()){
			std::ifstream in(path);
			json obj;
			in >> obj;
			in.close();

			Material mat(getTexture("res/textures/" + obj["diff"].get<std::string>()),
						 getTexture("res/textures/" + obj["spec"].get<std::string>()));
			MeshData* mesh = getMesh("res/models/" + obj["mesh"].get<std::string>());
			//std::vector<float> vec;
			//vec = obj["boxPos"].get<std::vector<float>>();
			//glm::vec3 pos = glm::vec3(vec[0], vec[1], vec[2]);
			//vec = obj["boxRot"].get<std::vector<float>>();
			//glm::vec3 rot = glm::vec3(vec[0], vec[1], vec[2]); 
			//vec = obj["boxScale"].get<std::vector<float>>();
			//glm::vec3 scale = glm::vec3(vec[0], vec[1], vec[2]);
			bool billboard = obj["billboard"].get<bool>();

			TexturedModel model = TexturedModel(mesh, mat, billboard);

			auto ret = m_modelsMap.insert(make_pair(path, model));
			return &(ret.first->second);
		}

		return &(it->second);
	}

	TexturedModel * ResourceManager::loadModel(int shape){
		return loadModel(IndexToShape(shape));
	}

	void ResourceManager::Init(){}

	void ResourceManager::ClearData(){
		for(auto mesh : m_meshesMap){
			glDeleteVertexArrays(1, &(mesh.second.vertexArrayObject));
			for(int i = 0; i < NUM_BUFFERS; i++){
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

	char* ResourceManager::IndexToShape(int index){
		char* shape;
		switch(index){
		case CollisionShapes::SHAPE_CUBE:
			shape = "cube";
			break;
		case CollisionShapes::SHAPE_SPHERE:
			shape = "sphere";
			break;
		case CollisionShapes::SHAPE_CILINDER:
			shape = "cilinder";
			break;
		case CollisionShapes::SHAPE_CONE:
			shape = "cone";
			break;
		case CollisionShapes::SHAPE_CAPSULE:
			shape = "capsule";
			break;
		default:
			shape = "";
			break;
		}
		return shape;
	}
}