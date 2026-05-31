#version 300 es

precision highp float;
precision highp int;

in vec2 vs_UV;

uniform sampler2D u_Tex;

out vec4 fs_Color;

void main()
{
    fs_Color = texture(u_Tex, vs_UV);
}