#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;
out float visibility;	//e^(-(distance * density)^gradient)

uniform mat4 view;
uniform mat4 projection;
uniform float density;
uniform float gradient;


void main(void){
	mat4 model;
	model[0] = vec4(1.0, 0.0, 0.0, 0.0);
	model[1] = vec4(0.0, 1.0, 0.0, 0.0);
	model[2] = vec4(0.0, 0.0, 1.0, 0.0);
	model[3] = vec4(0.0, 0.0, 0.0, 1.0);

	vec4 worldSpace = model * vec4(aPos, 1.0);
	vec4 eyeSpace = view * worldSpace;

	gl_Position = projection * eyeSpace;
	FragPos = vec3(worldSpace);
	Normal = mat3(transpose(inverse(model))) * aNormal;
	//Normal = (model * vec4(aNormal, 0.0)).xyz;
	TexCoords = aTexCoords;

	float distance = length(eyeSpace.xyz);
	visibility = exp(-pow((distance * density), gradient));
	visibility = clamp(visibility, 0.0, 1.0);
}