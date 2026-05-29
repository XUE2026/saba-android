#version 300 es

precision highp float;

in vec3 vs_Color;

out vec4 fs_Color;

void main()
{
	fs_Color.rgb = vs_Color;
	fs_Color.a = 1.0;
}