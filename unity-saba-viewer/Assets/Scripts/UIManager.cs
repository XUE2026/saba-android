using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

namespace UnitySabaViewer
{
    public class UIManager : MonoBehaviour
    {
        public static UIManager Instance { get; private set; }

        [Header("Canvas")]
        [SerializeField] private Canvas _mainCanvas;

        [Header("Panels")]
        [SerializeField] private GameObject _modelPanel;
        [SerializeField] private GameObject _filterPanel;
        [SerializeField] private GameObject _environmentPanel;
        [SerializeField] private GameObject _bonePanel;
        [SerializeField] private GameObject _animationPanel;
        [SerializeField] private GameObject _recordingPanel;
        [SerializeField] private GameObject _settingsPanel;
        [SerializeField] private GameObject _languagePanel;

        [Header("Model Panel")]
        [SerializeField] private Button _loadModelButton;
        [SerializeField] private Button _unloadModelButton;
        [SerializeField] private Text _modelStatusText;

        [Header("Filter Panel")]
        [SerializeField] private Dropdown _filterDropdown;
        [SerializeField] private Slider _exposureSlider;
        [SerializeField] private Slider _contrastSlider;
        [SerializeField] private Slider _saturationSlider;

        [Header("Environment Panel")]
        [SerializeField] private Dropdown _groundDropdown;
        [SerializeField] private Dropdown _primitiveDropdown;
        [SerializeField] private Button _addPrimitiveButton;
        [SerializeField] private Slider _sunIntensitySlider;

        [Header("Bone Panel")]
        [SerializeField] private Dropdown _boneDropdown;
        [SerializeField] private Slider _boneRotXSlider;
        [SerializeField] private Slider _boneRotYSlider;
        [SerializeField] private Slider _boneRotZSlider;
        [SerializeField] private Button _resetBoneButton;

        [Header("Animation Panel")]
        [SerializeField] private Button _idleButton;
        [SerializeField] private Button _walkButton;
        [SerializeField] private Button _runButton;
        [SerializeField] private Button _jumpButton;
        [SerializeField] private Button _stopButton;

        [Header("Recording Panel")]
        [SerializeField] private Button _startRecordButton;
        [SerializeField] private Button _stopRecordButton;
        [SerializeField] private Button _exportVmdButton;
        [SerializeField] private Button _exportGobotButton;

        [Header("Settings Panel")]
        [SerializeField] private Slider _cameraSensitivitySlider;
        [SerializeField] private Toggle _vsyncToggle;

        [Header("Language Panel")]
        [SerializeField] private Button _chineseButton;
        [SerializeField] private Button _englishButton;
        [SerializeField] private Button _japaneseButton;

        private Dictionary<string, GameObject> _panels = new Dictionary<string, GameObject>();
        private Dictionary<string, Text> _uiTexts = new Dictionary<string, Text>();

        private void Awake()
        {
            if (Instance != null && Instance != this)
            {
                Destroy(this);
                return;
            }
            Instance = this;

            if (_mainCanvas == null)
            {
                _mainCanvas = GetComponentInChildren<Canvas>();
                if (_mainCanvas == null)
                {
                    CreateMainCanvas();
                }
            }

            RegisterPanels();
            SetupDefaultUI();
        }

        private void CreateMainCanvas()
        {
            GameObject canvasObj = new GameObject("MainCanvas");
            canvasObj.transform.SetParent(transform);
            _mainCanvas = canvasObj.AddComponent<Canvas>();
            _mainCanvas.renderMode = RenderMode.ScreenSpaceOverlay;
            canvasObj.AddComponent<CanvasScaler>();
            canvasObj.AddComponent<GraphicRaycaster>();
        }

        private void RegisterPanels()
        {
            RegisterPanel("ModelPanel", _modelPanel);
            RegisterPanel("FilterPanel", _filterPanel);
            RegisterPanel("EnvironmentPanel", _environmentPanel);
            RegisterPanel("BonePanel", _bonePanel);
            RegisterPanel("AnimationPanel", _animationPanel);
            RegisterPanel("RecordingPanel", _recordingPanel);
            RegisterPanel("SettingsPanel", _settingsPanel);
            RegisterPanel("LanguagePanel", _languagePanel);
        }

        private void RegisterPanel(string name, GameObject panel)
        {
            if (panel != null)
            {
                _panels[name] = panel;
            }
        }

        private void SetupDefaultUI()
        {
            if (_filterDropdown != null)
            {
                _filterDropdown.ClearOptions();
                _filterDropdown.AddOptions(new List<string>(new[] { "WhiteBright", "SkinBright", "Dark", "GenshinDay", "GenshinNight", "GenshinDusk" }));
            }

            if (_groundDropdown != null)
            {
                _groundDropdown.ClearOptions();
                _groundDropdown.AddOptions(new List<string>(new[] { "Grid", "Checker", "Grass", "Dirt", "Asphalt" }));
            }

            if (_primitiveDropdown != null)
            {
                _primitiveDropdown.ClearOptions();
                _primitiveDropdown.AddOptions(new List<string>(new[] { "Cube", "Sphere", "Cone", "Cylinder" }));
            }
        }

        public void ShowPanel(string panelName)
        {
            if (_panels.TryGetValue(panelName, out GameObject panel) && panel != null)
            {
                panel.SetActive(true);
            }
        }

        public void HidePanel(string panelName)
        {
            if (_panels.TryGetValue(panelName, out GameObject panel) && panel != null)
            {
                panel.SetActive(false);
            }
        }

        public void TogglePanel(string panelName)
        {
            if (_panels.TryGetValue(panelName, out GameObject panel) && panel != null)
            {
                panel.SetActive(!panel.activeSelf);
            }
        }

        public void HideAllPanels()
        {
            foreach (var kvp in _panels)
            {
                if (kvp.Value != null)
                {
                    kvp.Value.SetActive(false);
                }
            }
        }

        public void ShowAllPanels()
        {
            foreach (var kvp in _panels)
            {
                if (kvp.Value != null)
                {
                    kvp.Value.SetActive(true);
                }
            }
        }

        public bool IsPanelVisible(string panelName)
        {
            if (_panels.TryGetValue(panelName, out GameObject panel) && panel != null)
            {
                return panel.activeSelf;
            }
            return false;
        }

        public void SetModelStatus(string status)
        {
            if (_modelStatusText != null)
            {
                _modelStatusText.text = status;
            }
        }

        public Button GetButton(string name)
        {
            switch (name)
            {
                case "LoadModel": return _loadModelButton;
                case "UnloadModel": return _unloadModelButton;
                case "Idle": return _idleButton;
                case "Walk": return _walkButton;
                case "Run": return _runButton;
                case "Jump": return _jumpButton;
                case "Stop": return _stopButton;
                case "StartRecord": return _startRecordButton;
                case "StopRecord": return _stopRecordButton;
                case "ExportVMD": return _exportVmdButton;
                case "ExportGobot": return _exportGobotButton;
                case "AddPrimitive": return _addPrimitiveButton;
                case "ResetBone": return _resetBoneButton;
                case "Chinese": return _chineseButton;
                case "English": return _englishButton;
                case "Japanese": return _japaneseButton;
                default:
                    Debug.LogWarning($"[UIManager] Unknown button: {name}");
                    return null;
            }
        }

        public Slider GetSlider(string name)
        {
            switch (name)
            {
                case "Exposure": return _exposureSlider;
                case "Contrast": return _contrastSlider;
                case "Saturation": return _saturationSlider;
                case "SunIntensity": return _sunIntensitySlider;
                case "BoneRotX": return _boneRotXSlider;
                case "BoneRotY": return _boneRotYSlider;
                case "BoneRotZ": return _boneRotZSlider;
                case "CameraSensitivity": return _cameraSensitivitySlider;
                default:
                    Debug.LogWarning($"[UIManager] Unknown slider: {name}");
                    return null;
            }
        }

        public Dropdown GetDropdown(string name)
        {
            switch (name)
            {
                case "Filter": return _filterDropdown;
                case "Ground": return _groundDropdown;
                case "Primitive": return _primitiveDropdown;
                case "Bone": return _boneDropdown;
                default:
                    Debug.LogWarning($"[UIManager] Unknown dropdown: {name}");
                    return null;
            }
        }
    }
}