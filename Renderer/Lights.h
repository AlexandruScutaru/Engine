#ifndef LIGHTS_H
#define LIGHTS_H

#include <glm/glm.hpp>


namespace renderer{
	enum Caster{
		DIRECTIONAL,
		POINT,
		SPOT
	};


	struct Light{
		Light(){}
		Light(glm::vec3& amb, glm::vec3& diff, glm::vec3& spec, int type){
			ambient = amb;
			diffuse = diff;
			specular = spec;
			casterType = type;
		}
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;

		int casterType;
	};


	struct DirLight : Light{
		DirLight(){}
		DirLight(glm::vec3& amb, glm::vec3& diff, glm::vec3& spec, glm::vec3& dir):
			Light(amb, diff, spec, Caster::DIRECTIONAL),
			direction(dir)
		{}
		
		glm::vec3 direction;
		
	};

	struct PointLight : Light{
		PointLight(){}
		PointLight(glm::vec3& amb, glm::vec3& diff, glm::vec3& spec, glm::vec3& pos, glm::vec3& att):
			Light(amb, diff, spec, Caster::POINT),
			position(pos),
			attenuation(att)
		{}

		glm::vec3 position;
		glm::vec3 attenuation;
	};

	struct SpotLight : Light{
		SpotLight(){}
		SpotLight(glm::vec3& amb, glm::vec3& diff, glm::vec3& spec, glm::vec3& dir, glm::vec3& pos, glm::vec3& att, float cut, float outerCut):
			Light(amb, diff, spec, Caster::SPOT),
			direction(dir),
			position(pos),
			attenuation(att),
			cutOff(cut),
			outerCutOff(outerCut)
		{}
		
		glm::vec3 direction;
		glm::vec3 position;
		glm::vec3 attenuation;
		float cutOff;
		float outerCutOff;
	};

}

#endif // !LIGHTS_H
