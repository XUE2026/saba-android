#version 300 es

layout(location = 0) in vec3 in_Pos;
layout(location = 1) in vec3 in_Nor;
#ifdef USE_UV
layout(location = 2) in vec2 in_UV;
#endif

uniform mat4 u_WV;
uniform mat4 u_WVP;
uniform mat3 u_WVIT;

out vec3 vs_Pos;
out vec3 vs_Nor;
#ifdef USE_UV
out vec2 vs_UV;
#endif

void main()
{
	gl_Position = u_WVP * vec4(in_Pos, 1.0);
	vs_Pos = (u_WV * vec4(in_Pos, 1.0)).xyz;
	vs_Nor = u_WVIT * in_Nor;
	#ifdef USE_UV
	vs_UV = in_UV;
	#endif
}