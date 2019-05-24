#include "Terrain.h"
#include "ModelDataStructs.h"
#include "ResourceManager.h"

#include <stdarg.h>


namespace renderer {

	Terrain::Terrain():
		m_mesh(nullptr),
		m_baseTexture(nullptr),
		m_redTexture(nullptr),
		m_greenTexture(nullptr),
		m_blueTexture(nullptr),
		m_blendTexture(nullptr),
		m_shader(nullptr),
		m_enabled(false)
	{
		m_terrainTextures = std::vector<std::string>(6, "");
		m_terrainTextures[HEIGHT] = "res/textures/terrain/default_heightMap.bmp";
		m_terrainTextures[BASE] = "res/textures/terrain/grass_dry.png";
		m_terrainTextures[RED] = "res/textures/terrain/grass.png";
		m_terrainTextures[GREEN] = "res/textures/terrain/dirt.png";
		m_terrainTextures[BLUE] = "res/textures/terrain/stone_ground.png";
		m_terrainTextures[BLEND] = "res/textures/terrain/default_blendMap.png";
		m_height_mult = 4.0f;
		m_side_size = 50.0f;

	}

	Terrain::~Terrain(){}

	void Terrain::render(int n, ...){
		m_shader->use();

		va_list args;
		va_start(args, n);
		m_shader->loadAdditionalData(n, args);
		va_end(args);

		glBindVertexArray(m_mesh->vertexArrayObject);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_baseTexture->id);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_redTexture->id);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_greenTexture->id);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, m_blueTexture->id);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, m_blendTexture->id);

		glDrawElements(GL_TRIANGLES, m_mesh->indexCount, GL_UNSIGNED_INT, 0);

		m_shader->unuse();
	}

	void Terrain::init(IShaderProgram * shader){
		m_shader = shader;
	}

	void Terrain::set(){
		m_mesh = utilities::ResourceManager::genTerrainHightField(m_terrainTextures[HEIGHT], m_side_size, m_height_mult);
		m_baseTexture = utilities::ResourceManager::getTexture(m_terrainTextures[BASE], false);
		m_redTexture = utilities::ResourceManager::getTexture(m_terrainTextures[RED], false);
		m_greenTexture = utilities::ResourceManager::getTexture(m_terrainTextures[GREEN], false);
		m_blueTexture = utilities::ResourceManager::getTexture(m_terrainTextures[BLUE], false);
		m_blendTexture = utilities::ResourceManager::getTexture(m_terrainTextures[BLEND], false);
	}

	void Terrain::setTerrainTexturePath(unsigned char index, const std::string & path){
		m_terrainTextures[index] = path;
		set();
	}

}