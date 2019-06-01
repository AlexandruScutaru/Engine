#version 330 core

in vec3 vUV;					// interpolated vertex output data
in vec2 vUVDot;					// interpolated vertex output data

out vec4 FragColor;

uniform sampler2D screenTexture;     // sampler of rendered scene’s render target

 
void main() {
    vec3 uv = dot(vUVDot, vUVDot) * vec3(-0.5, -0.5, -1.0) + vUV;
    FragColor = texture2DProj(screenTexture, uv);
}