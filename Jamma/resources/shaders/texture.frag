#version 330 core

in vec2 UV;

out vec3 ColorOUT;

uniform sampler2D TextureSampler;

void main(){

    ColorOUT = texture(TextureSampler, UV).rgb;
	//ColorOUT = vec4(1.0, 0.3, 0.3, 1.0);
}