#version 330 core

in vec2 TexCoords; 
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D diffuse;
uniform bool selected;

void main(){
	vec4 color = texture(diffuse, TexCoords);

	if(color.a < 0.5)
		discard;

    FragColor = color;
	if(selected)
		FragColor = clamp(FragColor * vec4(0.5, 0.2, 0.2, 1.0), 0.0, 1.0);
}