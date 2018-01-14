#ifndef TEXTURED_MODEL_H
#define TEXTURED_MODEL_H

#include "ModelDataStructs.h"
#include "Material.h"

namespace renderer{

	class TexturedModel{
		public:
			TexturedModel();
			TexturedModel(MeshData* mesh, Material& material);
			~TexturedModel();

			void setMesh(MeshData* mesh){ m_mesh = mesh; }
			void setMaterial(Material& material){ m_material = material; }

			MeshData* getMesh() { return m_mesh; }
			Material& getMaterial() { return m_material; }

		private:
			MeshData* m_mesh;
			Material m_material;
	};

}

#endif // !TEXTURED_MODEL_H
