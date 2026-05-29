using System;
using UnityEngine;

namespace Saba.MMDViewer
{
    public enum FilterType
    {
        None,
        BrightWhite,
        SkinBright,
        Dark,
        GenshinDay,
        GenshinNight,
        GenshinDusk
    }

    [Serializable]
    public struct FilterParameters
    {
        public float exposure;
        public float contrast;
        public float saturation;
        public float hueShift;
        public float temperature;
        public float tint;

        public static FilterParameters Default => new FilterParameters
        {
            exposure = 0f,
            contrast = 1f,
            saturation = 1f,
            hueShift = 0f,
            temperature = 0f,
            tint = 0f
        };

        public static FilterParameters Lerp(FilterParameters a, FilterParameters b, float t)
        {
            return new FilterParameters
            {
                exposure = Mathf.Lerp(a.exposure, b.exposure, t),
                contrast = Mathf.Lerp(a.contrast, b.contrast, t),
                saturation = Mathf.Lerp(a.saturation, b.saturation, t),
                hueShift = Mathf.Lerp(a.hueShift, b.hueShift, t),
                temperature = Mathf.Lerp(a.temperature, b.temperature, t),
                tint = Mathf.Lerp(a.tint, b.tint, t)
            };
        }
    }

    public class FilterManager : MonoBehaviour
    {
        [Header("Filter Settings")]
        [SerializeField] private FilterType currentFilter = FilterType.None;
        [SerializeField] private float intensity = 1f;
        [SerializeField] private FilterParameters currentParameters = FilterParameters.Default;
        [SerializeField] private Material filterMaterial;

        private FilterType previousFilter = FilterType.None;
        private FilterParameters targetParameters = FilterParameters.Default;
        private Material internalMaterial;

        private static readonly int ExposureParam = Shader.PropertyToID("_Exposure");
        private static readonly int ContrastParam = Shader.PropertyToID("_Contrast");
        private static readonly int SaturationParam = Shader.PropertyToID("_Saturation");
        private static readonly int HueShiftParam = Shader.PropertyToID("_HueShift");
        private static readonly int TemperatureParam = Shader.PropertyToID("_Temperature");
        private static readonly int TintParam = Shader.PropertyToID("_Tint");
        private static readonly int IntensityParam = Shader.PropertyToID("_Intensity");

        private void Awake()
        {
            if (filterMaterial != null)
            {
                internalMaterial = new Material(filterMaterial);
            }
            else
            {
                internalMaterial = new Material(Shader.Find("Saba/FilterShader"));
            }

            currentParameters = FilterParameters.Default;
            ApplyFilterParameters();
        }

        private void OnRenderImage(RenderTexture source, RenderTexture destination)
        {
            if (currentFilter == FilterType.None || internalMaterial == null || Mathf.Approximately(intensity, 0f))
            {
                Graphics.Blit(source, destination);
                return;
            }

            internalMaterial.SetFloat(IntensityParam, intensity);
            Graphics.Blit(source, destination, internalMaterial);
        }

        public void ApplyFilter(FilterType type)
        {
            previousFilter = currentFilter;
            currentFilter = type;
            targetParameters = GetFilterParameters(type);
        }

        public void SetIntensity(float value)
        {
            intensity = Mathf.Clamp01(value);
        }

        public float GetIntensity()
        {
            return intensity;
        }

        public void SetCustomParameters(float exposure, float contrast, float saturation,
            float hueShift, float temperature, float tint)
        {
            targetParameters = new FilterParameters
            {
                exposure = exposure,
                contrast = contrast,
                saturation = saturation,
                hueShift = hueShift,
                temperature = temperature,
                tint = tint
            };

            currentFilter = FilterType.None;
            UpdateParameters(targetParameters);
        }

        private void Update()
        {
            if (currentFilter != FilterType.None)
            {
                currentParameters = FilterParameters.Lerp(currentParameters, targetParameters, Time.deltaTime * 5f);
            }
            else if (previousFilter != FilterType.None)
            {
                currentParameters = FilterParameters.Lerp(currentParameters, FilterParameters.Default, Time.deltaTime * 5f);

                if (Vector4.Distance(
                        new Vector4(currentParameters.exposure, currentParameters.contrast,
                            currentParameters.saturation, currentParameters.hueShift),
                        new Vector4(0f, 1f, 1f, 0f)) < 0.01f)
                {
                    currentParameters = FilterParameters.Default;
                    previousFilter = FilterType.None;
                }
            }

            ApplyFilterParameters();
        }

        private void ApplyFilterParameters()
        {
            if (internalMaterial == null) return;

            internalMaterial.SetFloat(ExposureParam, currentParameters.exposure);
            internalMaterial.SetFloat(ContrastParam, currentParameters.contrast);
            internalMaterial.SetFloat(SaturationParam, currentParameters.saturation);
            internalMaterial.SetFloat(HueShiftParam, currentParameters.hueShift);
            internalMaterial.SetFloat(TemperatureParam, currentParameters.temperature);
            internalMaterial.SetFloat(TintParam, currentParameters.tint);
        }

        private void UpdateParameters(FilterParameters parameters)
        {
            currentParameters = parameters;
            ApplyFilterParameters();
        }

        public FilterType GetCurrentFilter()
        {
            return currentFilter;
        }

        public FilterParameters GetCurrentParameters()
        {
            return currentParameters;
        }

        public static FilterParameters GetFilterParameters(FilterType type)
        {
            switch (type)
            {
                case FilterType.BrightWhite:
                    return new FilterParameters
                    {
                        exposure = 0.3f,
                        contrast = 1.15f,
                        saturation = 0.8f,
                        hueShift = 0f,
                        temperature = -5f,
                        tint = 3f
                    };

                case FilterType.SkinBright:
                    return new FilterParameters
                    {
                        exposure = 0.2f,
                        contrast = 1.1f,
                        saturation = 1.05f,
                        hueShift = 2f,
                        temperature = 3f,
                        tint = 5f
                    };

                case FilterType.Dark:
                    return new FilterParameters
                    {
                        exposure = -0.4f,
                        contrast = 1.3f,
                        saturation = 0.7f,
                        hueShift = 0f,
                        temperature = -2f,
                        tint = -1f
                    };

                case FilterType.GenshinDay:
                    return new FilterParameters
                    {
                        exposure = 0.15f,
                        contrast = 1.05f,
                        saturation = 1.2f,
                        hueShift = 5f,
                        temperature = 8f,
                        tint = -2f
                    };

                case FilterType.GenshinNight:
                    return new FilterParameters
                    {
                        exposure = -0.5f,
                        contrast = 1.2f,
                        saturation = 0.9f,
                        hueShift = -5f,
                        temperature = -10f,
                        tint = 5f
                    };

                case FilterType.GenshinDusk:
                    return new FilterParameters
                    {
                        exposure = -0.1f,
                        contrast = 1.1f,
                        saturation = 1.15f,
                        hueShift = 10f,
                        temperature = 15f,
                        tint = -5f
                    };

                case FilterType.None:
                default:
                    return FilterParameters.Default;
            }
        }

        private void OnDestroy()
        {
            if (internalMaterial != null)
            {
                Destroy(internalMaterial);
            }
        }
    }
}