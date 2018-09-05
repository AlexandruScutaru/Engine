#ifndef MATERIAL_H
#define MATERIAL_H

#include "ModelDataStructs.h"


namespace renderer{

	class Material{
	public:
		Material();
		Material(TextureData* diff, TextureData* spec, float shininess = 32.0f);
		~Material();

		TextureData* getDiffuseMap(){ return m_diffuse; }
		TextureData* getSpecularMap(){ return m_specular; }
		float		 getShininess(){ return m_shininess; }

		void setDiffuseMap(TextureData* diff){ m_diffuse = diff; }
		void setSpecularMap(TextureData* spec){ m_specular = spec; }
		void setShininess(float shininess){ m_shininess = shininess; }

	private:
		TextureData* m_diffuse;
		TextureData* m_specular;
		float		 m_shininess;
	};

}
#endif // !MATERIAL_H
