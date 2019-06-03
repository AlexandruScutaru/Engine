#ifndef TERRAIN_H
#define TERRAIN_H

#include "IShaderProgram.h"
#include "PhysicsBody.h"

#include <memory>


namespace physics{
	class PhysicsWorld;
}

namespace renderer {

	struct TextureData;
	struct MeshData;

	class Terrain{
	public:
		Terrain();

		Terrain(const Terrain& other) = delete;
		Terrain& operator= (const Terrain& other) = delete;
		~Terrain();

		void render(int n, ...);

		enum : unsigned char{
			HEIGHT,
			BASE,
			RED,
			GREEN,
			BLUE,
			BLEND
		};

		void init(IShaderProgram* shader);

		void set();
		void setSize(float size){ m_side_size = size; }
		void setHeightMultiplier(float height_mult){ m_height_mult = height_mult; }
		void setEnabled(bool val) { m_enabled = val; }
		void setTerrainTexturePath(unsigned char index, const std::string& path);

		float* getPtrFirstElemOfHeights(){ return &m_heights[0]; }
		std::vector<float> getHeights(){ return m_heights; }

		float getMinH(){ return m_min; }
		float getMaxH(){ return m_max; }
		int getNumRows(){ return m_numRows; }

		std::string getTerrainTexturePath(unsigned char index) { return m_terrainTextures[index]; }
		float& getSizeRef(){ return m_side_size; }
		float& getHeightMultiplierRef(){ return m_height_mult; }
		bool enabled(){ return m_enabled; }
		bool& getEnabledRef(){ return m_enabled; }

		void setupPhysics(physics::PhysicsWorld* world);
		
		int bind();

	private:
		MeshData* m_mesh;
		TextureData* m_baseTexture;
		TextureData* m_redTexture;
		TextureData* m_greenTexture;
		TextureData* m_blueTexture;
		TextureData* m_blendTexture;

		IShaderProgram* m_shader;

		std::vector<std::string> m_terrainTextures;
		bool m_enabled;
		float m_height_mult;
		float m_side_size;
		std::vector<float> m_heights;
		float m_min, m_max;
		int m_numRows;

		std::shared_ptr<physics::PhysicsBody> m_rigidBody;
		physics::PhysicsWorld* m_pWorld;
	};

}

#endif // !TERRAIN_H
