#include "ResourceManager.h"

namespace renderer{

	std::map<std::string, TextureData> ResourceManager::m_texturesMap;
	std::map<std::string, MeshData> ResourceManager::m_meshesMap;
	std::vector<TexturedModel> ResourceManager::m_texturedModels;

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

	unsigned int ResourceManager::addTexturedModel(TexturedModel& model){
		m_texturedModels.push_back(model);
		return m_texturedModels.size() - 1;
	}

	TexturedModel * ResourceManager::getTexturedModelAt(unsigned int index){
		if(index < 0 || index >= m_texturedModels.size()){
			return nullptr;
		} else{
			return &(m_texturedModels[index]);
		}
	}

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
		
		m_texturedModels.clear();
	}
}