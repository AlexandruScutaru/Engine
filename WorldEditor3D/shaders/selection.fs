#version 330 core

out vec4 FragColor;

uniform vec3 colorCode;


void main(){
	FragColor = vec4(colorCode.r, 0.0, 0.0, 1.0);
}
