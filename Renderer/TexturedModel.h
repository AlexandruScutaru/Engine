#ifndef TEXTURED_MODEL_H
#define TEXTURED_MODEL_H

#include "ModelDataStructs.h"
#include "Material.h"
#include <GLM/glm.hpp>

namespace renderer{

	class TexturedModel{
		public:
			TexturedModel();
			TexturedModel(MeshData* mesh, Material& material, bool billboard = false, 
						  glm::vec3& pos = glm::vec3(0.0f), 
						  glm::vec3& rot = glm::vec3(0.0f),
						  glm::vec3& scale = glm::vec3(1.0f));
			~TexturedModel();

			void setMesh(MeshData* mesh){ m_mesh = mesh; }
			void setMaterial(Material& material){ m_material = material; }
			void setBillboard(bool billboard){ m_isBillboard = billboard; }

			MeshData* getMesh() { return m_mesh; }
			Material& getMaterial() { return m_material; }
			bool isBillboard(){ return m_isBillboard; }

		private:
			MeshData* m_mesh;
			Material m_material;
			bool m_isBillboard;

			glm::vec3 m_bbPosition;
			glm::vec3 m_bbRotation;
			glm::vec3 m_bbScale;
	};

}

#endif // !TEXTURED_MODEL_H
