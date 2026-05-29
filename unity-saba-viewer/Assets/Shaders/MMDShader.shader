Shader "Saba/MMDShader"
{
    Properties
    {
        [Header(Main Texture)]
        _MainTex ("Main Texture", 2D) = "white" {}
        _Color ("Main Color", Color) = (1, 1, 1, 1)

        [Header(Sphere Map)]
        _SphereMap ("Sphere Map", 2D) = "black" {}
        _SphereMapRate ("Sphere Map Rate", Range(0, 1)) = 0.5

        [Header(Toon Shading)]
        _ToonTex ("Toon Texture", 2D) = "white" {}
        _ToonRate ("Toon Rate", Range(0, 1)) = 0.5
        _ToonShadowColor ("Shadow Color", Color) = (0.5, 0.5, 0.5, 1)
        _ToonShadowRange ("Shadow Range", Range(0, 1)) = 0.5
        _ToonShadowSharpness ("Shadow Sharpness", Range(0.1, 10)) = 2.0

        [Header(Edge/Outline)]
        _EdgeColor ("Edge Color", Color) = (0, 0, 0, 1)
        _EdgeWidth ("Edge Width", Range(0, 0.1)) = 0.02
        _EdgeIntensity ("Edge Intensity", Range(0, 5)) = 1.0

        [Header(Lighting)]
        _LightIntensity ("Light Intensity", Range(0, 2)) = 1.0
        _AmbientIntensity ("Ambient Intensity", Range(0, 2)) = 0.3
        _SpecularColor ("Specular Color", Color) = (1, 1, 1, 1)
        _SpecularPower ("Specular Power", Range(1, 100)) = 20
        _SpecularIntensity ("Specular Intensity", Range(0, 2)) = 0.5

        [Header(Alpha)]
        _Alpha ("Alpha", Range(0, 1)) = 1
        _Cutoff ("Alpha Cutoff", Range(0, 1)) = 0.5

        [Header(Other)]
        _Outline ("Outline Mode", Float) = 0
        _Cull ("Cull Mode", Float) = 2
    }

    SubShader
    {
        Tags
        {
            "RenderType" = "Opaque"
            "Queue" = "Geometry"
            "LightMode" = "ForwardBase"
        }

        Pass
        {
            Name "FORWARD"
            Tags { "LightMode" = "ForwardBase" }

            Cull [_Cull]
            Blend SrcAlpha OneMinusSrcAlpha
            ZWrite On
            ZTest LEqual

            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            #pragma multi_compile_fwdbase
            #pragma multi_compile_fog
            #pragma target 3.0

            #include "UnityCG.cginc"
            #include "AutoLight.cginc"
            #include "Lighting.cginc"

            struct appdata
            {
                float4 vertex : POSITION;
                float3 normal : NORMAL;
                float2 uv : TEXCOORD0;
                float2 uv2 : TEXCOORD1;
            };

            struct v2f
            {
                float4 pos : SV_POSITION;
                float2 uv : TEXCOORD0;
                float2 uv2 : TEXCOORD1;
                float3 worldNormal : TEXCOORD2;
                float3 worldPos : TEXCOORD3;
                float3 viewDir : TEXCOORD4;
                SHADOW_COORDS(5)
                UNITY_FOG_COORDS(6)
            };

            sampler2D _MainTex;
            float4 _MainTex_ST;
            fixed4 _Color;

            sampler2D _SphereMap;
            float _SphereMapRate;

            sampler2D _ToonTex;
            float _ToonRate;
            fixed4 _ToonShadowColor;
            float _ToonShadowRange;
            float _ToonShadowSharpness;

            fixed4 _EdgeColor;
            float _EdgeWidth;
            float _EdgeIntensity;

            float _LightIntensity;
            float _AmbientIntensity;
            fixed4 _SpecularColor;
            float _SpecularPower;
            float _SpecularIntensity;

            float _Alpha;
            float _Cutoff;
            float _Outline;

            v2f vert(appdata v)
            {
                v2f o;

                if (_Outline > 0.5)
                {
                    float3 normal = normalize(v.normal);
                    float3 outlinePos = v.vertex.xyz + normal * _EdgeWidth * _EdgeIntensity;
                    o.pos = UnityObjectToClipPos(float4(outlinePos, 1));
                }
                else
                {
                    o.pos = UnityObjectToClipPos(v.vertex);
                }

                o.uv = TRANSFORM_TEX(v.uv, _MainTex);
                o.uv2 = v.uv2;

                float3 worldNormal = UnityObjectToWorldNormal(v.normal);
                o.worldNormal = normalize(worldNormal);
                o.worldPos = mul(unity_ObjectToWorld, v.vertex).xyz;
                o.viewDir = normalize(UnityWorldSpaceViewDir(o.worldPos));

                TRANSFER_SHADOW(o);
                UNITY_TRANSFER_FOG(o, o.pos);

                return o;
            }

            fixed4 frag(v2f i) : SV_Target
            {
                if (_Outline > 0.5)
                {
                    UNITY_APPLY_FOG(i.fogCoord, _EdgeColor);
                    return fixed4(_EdgeColor.rgb, _Alpha);
                }

                fixed4 mainTex = tex2D(_MainTex, i.uv) * _Color;

                clip(mainTex.a - _Cutoff);

                float3 normal = normalize(i.worldNormal);
                float3 lightDir = normalize(_WorldSpaceLightPos0.xyz);
                float3 viewDir = normalize(i.viewDir);
                float3 halfDir = normalize(lightDir + viewDir);

                float NdotL = dot(normal, lightDir) * _LightIntensity;
                float shadow = SHADOW_ATTENUATION(i);

                float toonIntensity = smoothstep(
                    _ToonShadowRange - 0.1 / _ToonShadowSharpness,
                    _ToonShadowRange + 0.1 / _ToonShadowSharpness,
                    NdotL * shadow
                );

                fixed4 toonTex = tex2D(_ToonTex, float2(toonIntensity, 0.5));
                fixed4 toonShading = lerp(_ToonShadowColor, fixed4(1, 1, 1, 1), toonTex.r);
                toonShading = lerp(fixed4(1, 1, 1, 1), toonShading, _ToonRate);

                float ambient = _AmbientIntensity;
                fixed4 ambientColor = fixed4(ambient, ambient, ambient, 1);

                float3 specular = float3(0, 0, 0);
                if (_SpecularIntensity > 0)
                {
                    float NdotH = max(0, dot(normal, halfDir));
                    specular = _SpecularColor.rgb * _SpecularIntensity * pow(NdotH, _SpecularPower);
                }

                fixed4 sphereTex = tex2D(_SphereMap, i.uv);
                half3 sphereColor = sphereTex.rgb * _SphereMapRate;

                fixed4 finalColor = mainTex * ambientColor * toonShading;
                finalColor.rgb += specular;
                finalColor.rgb += sphereColor;
                finalColor.rgb *= _LightColor0.rgb;

                UNITY_APPLY_FOG(i.fogCoord, finalColor);

                finalColor.a *= _Alpha;

                return finalColor;
            }
            ENDCG
        }

        Pass
        {
            Name "SHADOW_CASTER"
            Tags { "LightMode" = "ShadowCaster" }

            Cull Off
            ZWrite On
            ZTest LEqual

            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            #pragma multi_compile_shadowcaster
            #pragma target 3.0

            #include "UnityCG.cginc"

            struct v2f
            {
                V2F_SHADOW_CASTER;
                float2 uv : TEXCOORD1;
            };

            sampler2D _MainTex;
            float4 _MainTex_ST;
            float _Cutoff;

            v2f vert(appdata_base v)
            {
                v2f o;
                TRANSFER_SHADOW_CASTER_NORMALOFFSET(o)
                o.uv = TRANSFORM_TEX(v.texcoord, _MainTex);
                return o;
            }

            float4 frag(v2f i) : SV_Target
            {
                fixed4 texColor = tex2D(_MainTex, i.uv);
                clip(texColor.a - _Cutoff);
                SHADOW_CASTER_FRAGMENT(i)
            }
            ENDCG
        }
    }

    FallBack "Standard"
    CustomEditor "MMDShaderGUI"
}