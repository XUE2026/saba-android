using System;
using UnityEngine;
using UnityEngine.Rendering.PostProcessing;

namespace UnitySabaViewer
{
    [Serializable]
    public struct FilterPreset
    {
        public string name;
        public float exposure;
        public float contrast;
        public float saturation;
        public float hueShift;
        public float temperature;
        public float tint;

        public FilterPreset(string name, float exposure, float contrast, float saturation,
                           float hueShift, float temperature, float tint)
        {
            this.name = name;
            this.exposure = exposure;
            this.contrast = contrast;
            this.saturation = saturation;
            this.hueShift = hueShift;
            this.temperature = temperature;
            this.tint = tint;
        }
    }

    public class FilterManager : MonoBehaviour
    {
        public static FilterManager Instance { get; private set; }

        [Header("Post Processing")]
        [SerializeField] private PostProcessVolume _postProcessVolume;

        private ColorGrading _colorGrading;
        private int _currentPresetIndex = 0;

        public static readonly FilterPreset[] Presets = new FilterPreset[]
        {
            new FilterPreset("WhiteBright",    0.15f,  0.05f,  0.10f,  0f,     -5f,    0f),
            new FilterPreset("SkinBright",     0.20f,  -0.05f, 0.05f,  2f,     10f,    0f),
            new FilterPreset("Dark",           -0.30f, 0.15f,  -0.10f, 0f,     -10f,   0f),
            new FilterPreset("GenshinDay",     0.10f,  0.05f,  0.15f,  5f,     15f,    5f),
            new FilterPreset("GenshinNight",   -0.25f, 0.10f,  -0.05f, -3f,    -20f,   -5f),
            new FilterPreset("GenshinDusk",    -0.10f, 0.08f,  0.05f,  8f,     25f,    10f),
        };

        public int CurrentPresetIndex => _currentPresetIndex;
        public FilterPreset CurrentPreset => Presets[_currentPresetIndex];

        private void Awake()
        {
            if (Instance != null && Instance != this)
            {
                Destroy(this);
                return;
            }
            Instance = this;

            if (_postProcessVolume == null)
            {
                _postProcessVolume = GetComponent<PostProcessVolume>();
            }

            if (_postProcessVolume == null)
            {
                GameObject volumeObj = new GameObject("PostProcessVolume");
                volumeObj.transform.SetParent(transform);
                _postProcessVolume = volumeObj.AddComponent<PostProcessVolume>();
                _postProcessVolume.isGlobal = true;
                _postProcessVolume.weight = 1f;
            }

            _postProcessVolume.profile = new PostProcessProfile();
            _colorGrading = _postProcessVolume.profile.AddSettings<ColorGrading>();
        }

        public void ApplyPreset(int index)
        {
            if (index < 0 || index >= Presets.Length)
            {
                Debug.LogWarning($"[FilterManager] Invalid preset index: {index}");
                return;
            }

            _currentPresetIndex = index;
            ApplyPreset(Presets[index]);
        }

        public void ApplyPreset(string presetName)
        {
            for (int i = 0; i < Presets.Length; i++)
            {
                if (Presets[i].name.Equals(presetName, StringComparison.OrdinalIgnoreCase))
                {
                    ApplyPreset(i);
                    return;
                }
            }
            Debug.LogWarning($"[FilterManager] Preset not found: {presetName}");
        }

        private void ApplyPreset(FilterPreset preset)
        {
            if (_colorGrading == null) return;

            _colorGrading.exposure.value = preset.exposure;
            _colorGrading.contrast.value = preset.contrast;
            _colorGrading.saturation.value = preset.saturation;
            _colorGrading.hueShift.value = preset.hueShift;
            _colorGrading.temperature.value = preset.temperature;
            _colorGrading.tint.value = preset.tint;

            Debug.Log($"[FilterManager] Applied preset: {preset.name}");
        }

        public void SetExposure(float value)
        {
            if (_colorGrading != null)
            {
                _colorGrading.exposure.value = value;
            }
        }

        public void SetContrast(float value)
        {
            if (_colorGrading != null)
            {
                _colorGrading.contrast.value = value;
            }
        }

        public void SetSaturation(float value)
        {
            if (_colorGrading != null)
            {
                _colorGrading.saturation.value = value;
            }
        }

        public void SetHueShift(float value)
        {
            if (_colorGrading != null)
            {
                _colorGrading.hueShift.value = value;
            }
        }

        public void SetTemperature(float value)
        {
            if (_colorGrading != null)
            {
                _colorGrading.temperature.value = value;
            }
        }

        public void SetTint(float value)
        {
            if (_colorGrading != null)
            {
                _colorGrading.tint.value = value;
            }
        }

        public FilterPreset GetPreset(int index)
        {
            if (index >= 0 && index < Presets.Length)
                return Presets[index];
            return Presets[0];
        }

        public string[] GetPresetNames()
        {
            string[] names = new string[Presets.Length];
            for (int i = 0; i < Presets.Length; i++)
            {
                names[i] = Presets[i].name;
            }
            return names;
        }
    }
}