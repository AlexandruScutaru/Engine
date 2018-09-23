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
			MeshData* getMesh() { return m_mesh; }
			Material& getMaterial() { return m_material; }
			bool isBillboard(){ return m_isBillboard; }
			bool& isBillboardRef(){ return m_isBillboard; }

		private:
			MeshData* m_mesh;
			Material m_material;
			bool m_isBillboard;

	};

}

#endif // !TEXTURED_MODEL_H
