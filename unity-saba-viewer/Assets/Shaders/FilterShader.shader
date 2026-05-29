Shader "Saba/FilterShader"
{
    Properties
    {
        _MainTex ("Source", 2D) = "white" {}
        _Exposure ("Exposure", Range(-2, 2)) = 0
        _Contrast ("Contrast", Range(0, 3)) = 1
        _Saturation ("Saturation", Range(0, 3)) = 1
        _HueShift ("Hue Shift", Range(-180, 180)) = 0
        _Temperature ("Temperature", Range(-100, 100)) = 0
        _Tint ("Tint", Range(-100, 100)) = 0
        _Intensity ("Intensity", Range(0, 1)) = 1
    }

    SubShader
    {
        Cull Off
        ZWrite Off
        ZTest Always

        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            #pragma target 3.0

            #include "UnityCG.cginc"

            struct appdata
            {
                float4 vertex : POSITION;
                float2 uv : TEXCOORD0;
            };

            struct v2f
            {
                float4 pos : SV_POSITION;
                float2 uv : TEXCOORD0;
            };

            sampler2D _MainTex;
            float4 _MainTex_ST;
            float _Exposure;
            float _Contrast;
            float _Saturation;
            float _HueShift;
            float _Temperature;
            float _Tint;
            float _Intensity;

            v2f vert(appdata v)
            {
                v2f o;
                o.pos = UnityObjectToClipPos(v.vertex);
                o.uv = TRANSFORM_TEX(v.uv, _MainTex);
                return o;
            }

            float3 ApplyExposure(float3 color, float exposure)
            {
                return color * exp2(exposure);
            }

            float3 ApplyContrast(float3 color, float contrast)
            {
                float3 mid = float3(0.5, 0.5, 0.5);
                return (color - mid) * contrast + mid;
            }

            float3 ApplySaturation(float3 color, float saturation)
            {
                float luminance = dot(color, float3(0.2126, 0.7152, 0.0722));
                return lerp(float3(luminance, luminance, luminance), color, saturation);
            }

            float3 RGBToHSV(float3 rgb)
            {
                float4 K = float4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
                float4 p = lerp(float4(rgb.bg, K.w, K.z), float4(rgb.gb, K.x, K.y), step(rgb.b, rgb.g));
                float4 q = lerp(float4(p.xyw, rgb.r), float4(rgb.r, p.yzx), step(p.x, rgb.r));
                float d = q.x - min(q.w, q.y);
                float e = 1.0e-10;
                return float3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
            }

            float3 HSVToRGB(float3 hsv)
            {
                float4 K = float4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
                float3 p = abs(frac(hsv.xxx + K.xyz) * 6.0 - K.www);
                return hsv.z * lerp(K.xxx, clamp(p - K.xxx, 0.0, 1.0), hsv.y);
            }

            float3 ApplyHueShift(float3 color, float hueShift)
            {
                float3 hsv = RGBToHSV(color);
                hsv.x = frac(hsv.x + hueShift / 360.0);
                return HSVToRGB(hsv);
            }

            float3 ApplyTemperature(float3 color, float temperature)
            {
                float t = temperature / 100.0;
                float3 warm = float3(1.0, 0.9, 0.7);
                float3 cool = float3(0.7, 0.8, 1.0);

                float3 tempColor;
                if (t > 0)
                {
                    tempColor = lerp(float3(1, 1, 1), warm, t);
                }
                else
                {
                    tempColor = lerp(float3(1, 1, 1), cool, -t);
                }

                return color * tempColor;
            }

            float3 ApplyTint(float3 color, float tint)
            {
                float t = tint / 100.0;
                float3 greenTint = float3(0.8, 1.0, 0.9);
                float3 magentaTint = float3(1.0, 0.8, 1.0);

                float3 tintColor;
                if (t > 0)
                {
                    tintColor = lerp(float3(1, 1, 1), magentaTint, t);
                }
                else
                {
                    tintColor = lerp(float3(1, 1, 1), greenTint, -t);
                }

                return color * tintColor;
            }

            float3 LinearToSRGB(float3 linear)
            {
                return linear < 0.0031308
                    ? linear * 12.92
                    : 1.055 * pow(linear, 1.0 / 2.4) - 0.055;
            }

            float3 SRGBToLinear(float3 srgb)
            {
                return srgb < 0.04045
                    ? srgb / 12.92
                    : pow((srgb + 0.055) / 1.055, 2.4);
            }

            fixed4 frag(v2f i) : SV_Target
            {
                fixed4 col = tex2D(_MainTex, i.uv);

                float3 linearColor = col.rgb;

                linearColor = ApplyExposure(linearColor, _Exposure * _Intensity);
                linearColor = ApplyContrast(linearColor, 1.0 + (_Contrast - 1.0) * _Intensity);
                linearColor = ApplySaturation(linearColor, 1.0 + (_Saturation - 1.0) * _Intensity);
                linearColor = ApplyHueShift(linearColor, _HueShift * _Intensity);
                linearColor = ApplyTemperature(linearColor, _Temperature * _Intensity);
                linearColor = ApplyTint(linearColor, _Tint * _Intensity);

                col.rgb = linearColor;

                return col;
            }
            ENDCG
        }
    }

    FallBack Off
}