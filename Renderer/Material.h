#ifndef MATERIAL_H
#define MATERIAL_H

#include "ModelDataStructs.h"


namespace renderer{

	class Material{
	public:
		Material();
		Material(TextureData* diff, TextureData* spec);
		~Material();

		TextureData* getDiffuseMap(){ return m_diffuse; }
		TextureData* getSpecularMap(){ return m_specular; }

		void setDiffuseMap(TextureData* diff){ m_diffuse = diff; }
		void setSpecularMap(TextureData* spec){ m_specular = spec; }

	private:
		TextureData* m_diffuse;
		TextureData* m_specular;
	};

}
#endif // !MATERIAL_H
