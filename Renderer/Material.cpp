#include "Material.h"


namespace renderer{

	Material::Material():
		m_diffuse(nullptr),
		m_specular(nullptr)
	{}

	renderer::Material::Material(TextureData* diff, TextureData* spec):
		m_diffuse(diff),
		m_specular(spec)
	{}

	Material::~Material(){
		m_diffuse = nullptr;
		m_specular = nullptr;
	}

}