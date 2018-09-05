#include "Material.h"


namespace renderer{

	Material::Material():
		m_diffuse(nullptr),
		m_specular(nullptr),
		m_shininess(0.0f)
	{}

	renderer::Material::Material(TextureData* diff, TextureData* spec, float shininess) :
		m_diffuse(diff),
		m_specular(spec),
		m_shininess(shininess)
	{}

	Material::~Material(){
		m_diffuse = nullptr;
		m_specular = nullptr;
	}

}