#ifndef TEXTURED_MODEL_H
#define TEXTURED_MODEL_H

#include "Material.h"
#include <GLM/glm.hpp>

namespace renderer{
	
	struct MeshData;

	class TexturedModel{
		public:
			TexturedModel();
			TexturedModel(MeshData* mesh, Material& material, bool billboard = false);
			~TexturedModel();

			void setMesh(MeshData* mesh){ m_mesh = mesh; }
			void setMaterial(Material& material){ m_material = material; }
			void setBillboard(bool billboard){ m_isBillboard = billboard; }
			void setDoubleSided(bool doubleSided){ m_doubleSided = doubleSided; }
			void setAtlasSize(int size){ m_numRows = size; }
			
			MeshData* getMesh() { return m_mesh; }
			Material& getMaterial() { return m_material; }
			bool isBillboard(){ return m_isBillboard; }
			bool& isBillboardRef(){ return m_isBillboard; }
			bool isDoubleSided(){ return m_doubleSided; }
			bool& isDoubleSidedRef(){ return m_doubleSided; }
			int getAtlasSize(){ return m_numRows; }
			int& getAtlasSizeRef(){ return m_numRows; }

		private:
			MeshData* m_mesh;
			Material m_material;
			bool m_isBillboard;
			bool m_doubleSided;
			int m_numRows;

	};

}

#endif // !TEXTURED_MODEL_H
