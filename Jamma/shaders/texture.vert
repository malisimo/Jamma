#version 330 core

layout(location = 0) in vec3 PositionIN;
layout(location = 1) in vec2 UvIN;

out vec2 UV;

uniform mat4 MVP;

void main(){

    // Output position of the vertex, in clip space : MVP * position
    //gl_Position =  MVP * vec4(PositionIN,1);
    gl_Position = vec4(PositionIN,1);

    // UV of the vertex. No special space for this one.
    UV = UvIN;
}