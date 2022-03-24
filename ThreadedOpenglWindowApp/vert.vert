#version 450 core

layout(location=0) in vec4 pos;
layout(location=1) in vec4 color;
layout(location=2) in vec2 textCoord;

uniform mat4 mvp;

out vec4 vertColor;
out vec2 textCoordV;

void main(void)
{
    gl_Position = mvp * pos;
    vertColor = color;
    textCoordV = textCoord;
}
