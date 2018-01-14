#include "TexturedModel.h"

namespace renderer{

	TexturedModel::TexturedModel():
		m_mesh(nullptr)
	{}

	TexturedModel::TexturedModel(MeshData* mesh, Material& material):
		m_mesh(mesh),
		m_material(material)
	{}

	TexturedModel::~TexturedModel(){
		m_mesh = nullptr;
	}

}