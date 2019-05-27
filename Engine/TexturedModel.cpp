#include "TexturedModel.h"

namespace renderer{

	TexturedModel::TexturedModel():
		m_mesh(nullptr),
		m_isBillboard(false),
		m_numRows(1)
	{}

	TexturedModel::TexturedModel(MeshData* mesh, Material& material, bool billboard) :
		m_mesh(mesh),
		m_material(material),
		m_isBillboard(billboard),
		m_numRows(1)
	{}

	TexturedModel::~TexturedModel(){
		m_mesh = nullptr;
	}

}