#version 450 core

in vec4 vertColor;
in vec2 textCoordV;
out vec4 finalColor;

uniform sampler2D tex;

void main(void)
{
    //gl_FragColor = vertColor;
    //finalColor = vertColor;
    finalColor = texture(tex, textCoordV);
}
