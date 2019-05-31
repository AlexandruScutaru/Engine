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

	renderer::MeshData* ResourceManager::genTerrainHightField(const std::string & heightFieldPath, float side_size, float height_mult, int& num_rows, std::vector<float>& heights, float& min, float& max){
		auto it = m_meshesMap.find(heightFieldPath);
		if(it != m_meshesMap.end()){
			glDeleteVertexArrays(1, &(it->second.vertexArrayObject));
			for(int i = 0; i < renderer::NUM_BUFFERS; i++){
				glDeleteBuffers(1, &(it->second.vertexArrayBuffers[i]));
			}
		}
		renderer::MeshData mesh = utilities::ObjectLoader::loadTerrainHeightField(heightFieldPath, side_size, height_mult, num_rows, heights, min, max);
		auto ret = m_meshesMap.insert(make_pair(heightFieldPath, mesh));
		return &(ret.first->second);
	}

	renderer::MeshData * ResourceManager::getNDCquad(){
		auto it = m_meshesMap.find("NDCquad");
		if(it == m_meshesMap.end()){
			float quad[] = { 
				// positions	// texCoords
				-1.0f, 1.0f,	0.0f, 1.0f,
				-1.0f, -1.0f,	0.0f, 0.0f,
				1.0f, -1.0f,	1.0f, 0.0f,

				-1.0f, 1.0f,	0.0f, 1.0f,
				1.0f, -1.0f,	1.0f, 0.0f,
				1.0f, 1.0f,		1.0f, 1.0f
			};
			renderer::MeshData mesh;
			glGenVertexArrays(1, &mesh.vertexArrayObject);
			glGenBuffers(1, &mesh.vertexArrayBuffers[0]);
			glBindVertexArray(mesh.vertexArrayObject);
			glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexArrayBuffers[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
			mesh.indexCount = 6;
			auto ret = m_meshesMap.insert(make_pair(std::string("NDCquad"), mesh));
			return &(ret.first->second);
		}
		return &(it->second);
	}

	renderer::TextureData* ResourceManager::getTexture(const std::string& path, bool flip){
		//lookup the texture and see if its in the map
		auto it = m_texturesMap.find(path); //auto looks for the return type and automatically assigns it
											//here auto is std::map<std::string, GLTexture>::iterator
		if(it == m_texturesMap.end()){
			//Load the texture
			renderer::TextureData newTexture = TextureLoader::loadTexture(path, flip);

			auto ret = m_texturesMap.insert(make_pair(path, newTexture));

			return &(ret.first->second);
		}
		return &(it->second);
	}

	GLuint ResourceManager::getCubemapTexture(std::vector<std::string>& paths){
		return TextureLoader::loadCubeMapTexture(paths);
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
			model.setAtlasSize(obj["atlasSize"].get<int>());
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