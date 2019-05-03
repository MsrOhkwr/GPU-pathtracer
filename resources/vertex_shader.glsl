#version 450 core

attribute vec3 position_vertices;
varying vec2 position_screen;

void main()
{
	position_screen = position_vertices.xy / 2.0f + 0.5f;
	gl_Position = vec4(position_vertices, 1.0f);
}