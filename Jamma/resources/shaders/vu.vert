#version 330 core

layout(location = 0) in vec3 PositionIN;
layout(location = 1) in vec2 UvIN;

out vec2 UV;
out float LedAlpha;
out vec3 Rgb;

uniform mat4 MVP;
uniform float Value;

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
    gl_Position =  MVP * vec4(PositionIN,1);
    UV = UvIN;
	LedAlpha = sign(Value - UvIN.y) * .5 + .5;
	Rgb = hsv2rgb(vec3(1.4 - (UvIN.y * 0.4), 1., 1.));
}