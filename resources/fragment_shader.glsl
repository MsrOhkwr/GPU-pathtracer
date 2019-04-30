#version 450 core

varying vec2 position_screen;
layout(binding = 1) uniform sampler2D img_output;
out vec4 color;

void main()
{
    color = texture(img_output, position_screen);
}