#ifndef LIGHTS_H
#define LIGHTS_H

#include <glm/glm.hpp>


namespace renderer{

	struct DirLight{
		DirLight(){}
		DirLight(glm::vec3& dir, glm::vec3& amb, glm::vec3& diff, glm::vec3& spec){
			direction = dir;
			ambient = amb;
			diffuse = diff;
			specular = spec;
		}
		glm::vec3 direction;
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};

	struct PointLight{
		PointLight(){}
		PointLight(glm::vec3& pos, glm::vec3& amb, glm::vec3& diff, glm::vec3& spec, glm::vec3& att){
			position = pos;
			ambient = amb;
			diffuse = diff;
			specular = spec;
			attenuation = att;
		}
		glm::vec3 position;
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		glm::vec3 attenuation;
	};

	struct SpotLight{
		SpotLight(){}
		SpotLight(glm::vec3& amb, glm::vec3& diff, glm::vec3& spec, glm::vec3& att, glm::vec3& pos, glm::vec3& dir, float cut, float outerCut){
			ambient = amb;
			diffuse = diff;
			specular = spec;
			attenuation = att;
			position = pos;
			direction = dir;
			cutOff = cut;
			outerCutOff = outerCut;
		}
		float cutOff;
		float outerCutOff;
		glm::vec3 position;
		glm::vec3 direction;
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		glm::vec3 attenuation;
		
	};

}

#endif // !LIGHTS_H
