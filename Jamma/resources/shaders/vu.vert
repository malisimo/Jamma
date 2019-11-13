#version 330 core

layout(location = 0) in vec3 PositionIN;
layout(location = 1) in vec2 UvIN;

out vec2 UV;
out float LedAlpha;

uniform mat4 MVP;
uniform float Value;

void main()
{
    gl_Position =  MVP * vec4(PositionIN,1);
    UV = UvIN;
	LedAlpha = sign(UvIN.y - Value) * .5 + .5;
}