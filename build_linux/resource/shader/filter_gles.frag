#version 300 es

precision highp float;

in vec2 vs_UV;

uniform sampler2D u_Tex;
uniform float u_Exposure;
uniform float u_Contrast;
uniform float u_Saturation;
uniform float u_HueShift;
uniform float u_Temperature;
uniform float u_Tint;
uniform float u_Intensity;

out vec4 fs_Color;

vec3 hueShift(vec3 color, float shift)
{
    const vec3 k = vec3(0.57735, 0.57735, 0.57735);
    float cosAngle = cos(shift);
    float sinAngle = sin(shift);
    vec3 temp = cross(k, color);
    return color + temp * sinAngle + cross(k, temp) * (1.0 - cosAngle);
}

vec3 colorTemperature(vec3 color, float temp, float tint)
{
    float r = color.r;
    float g = color.g;
    float b = color.b;

    r += temp * 0.01;
    b -= temp * 0.01;

    g += tint * 0.01;
    b -= tint * 0.005;
    r -= tint * 0.005;

    return vec3(r, g, b);
}

void main()
{
    vec3 color = texture(u_Tex, vs_UV).rgb;

    vec3 outColor = color;

    outColor = outColor * u_Exposure;

    outColor = (outColor - 0.5) * u_Contrast + 0.5;

    float luminance = dot(outColor, vec3(0.299, 0.587, 0.114));
    outColor = mix(vec3(luminance), outColor, u_Saturation);

    float hueRad = u_HueShift * 3.14159265 / 180.0;
    outColor = hueShift(outColor, hueRad);

    outColor = colorTemperature(outColor, u_Temperature, u_Tint);

    outColor = clamp(outColor, 0.0, 1.0);

    fs_Color = vec4(mix(color, outColor, u_Intensity), 1.0);
}