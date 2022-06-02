#version 450 core

in vec4 vertColor;
in vec2 textCoordV;
out vec4 finalColor;

uniform sampler2D tex;

void main(void)
{
    //gl_FragColor = vertColor;
    //finalColor = vertColor;
    const float ratio = 0.0;
    finalColor =(1.0f - ratio) * texture(tex, textCoordV) + ratio*vertColor;
    //finalColor = clamp(finalColor, ve, )
}
