#version 330 core

//in vec2 TexCoords; 
//in vec3 Normal;
//in vec3 FragPos;

out vec4 FragColor;

//uniform sampler2D diffuse;

void main(){
    //FragColor = texture(diffuse, TexCoords);
	FragColor = vec4(0.0, 0.0, 1.0, 0.0);
}