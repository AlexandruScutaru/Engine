#include "TexturedModel.h"

namespace renderer{

	TexturedModel::TexturedModel():
		m_mesh(nullptr),
		m_isBillboard(false)
	{}

	TexturedModel::TexturedModel(MeshData* mesh, Material& material, bool billboard) :
		m_mesh(mesh),
		m_material(material),
		m_isBillboard(billboard)
	{}

	TexturedModel::~TexturedModel(){
		m_mesh = nullptr;
	}

}