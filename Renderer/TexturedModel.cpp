#include "TexturedModel.h"

namespace renderer{

	TexturedModel::TexturedModel():
		m_mesh(nullptr),
		m_isBillboard(false)
	{}

	TexturedModel::TexturedModel(MeshData* mesh, Material& material, bool billboard,
								 glm::vec3& pos, glm::vec3& rot, glm::vec3& scale):
		m_mesh(mesh),
		m_material(material),
		m_isBillboard(billboard),
		m_bbPosition(pos),
		m_bbRotation(rot),
		m_bbScale(scale)
	{}

	TexturedModel::~TexturedModel(){
		m_mesh = nullptr;
	}

}