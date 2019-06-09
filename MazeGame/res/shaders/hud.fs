#version 330 core

in vec2 textureCoords;

out vec4 out_Colour;

uniform sampler2D hudTexture;


void main(void){
	out_Colour = texture(hudTexture, textureCoords);
}