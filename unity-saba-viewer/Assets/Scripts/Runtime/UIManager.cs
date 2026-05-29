using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

namespace Saba.MMDViewer
{
    public class UIManager : MonoBehaviour
    {
        [Header("UI Panels")]
        [SerializeField] private GameObject modelPanel;
        [SerializeField] private GameObject filterPanel;
        [SerializeField] private GameObject environmentPanel;
        [SerializeField] private GameObject bonePanel;
        [SerializeField] private GameObject animationPanel;
        [SerializeField] private GameObject recordingPanel;
        [SerializeField] private GameObject settingsPanel;

        [Header("Model Panel UI")]
        [SerializeField] private Button loadModelButton;
        [SerializeField] private Button unloadModelButton;
        [SerializeField] private Text modelStatusText;

        [Header("Filter Panel UI")]
        [SerializeField] private Dropdown filterDropdown;
        [SerializeField] private Slider filterIntensitySlider;
        [SerializeField] private Text filterIntensityText;

        [Header("Environment Panel UI")]
        [SerializeField] private Slider sunXSlider;
        [SerializeField] private Slider sunYSlider;
        [SerializeField] private Slider sunIntensitySlider;
        [SerializeField] private Dropdown groundDropdown;
        [SerializeField] private Dropdown primitiveDropdown;
        [SerializeField] private Button addPrimitiveButton;
        [SerializeField] private Button clearPrimitivesButton;

        [Header("Bone Panel UI")]
        [SerializeField] private Dropdown boneDropdown;
        [SerializeField] private Slider boneRotateXSlider;
        [SerializeField] private Slider boneRotateYSlider;
        [SerializeField] private Slider boneRotateZSlider;
        [SerializeField] private Button resetBoneButton;
        [SerializeField] private Button resetAllBonesButton;
        [SerializeField] private Text selectedBoneText;

        [Header("Animation Panel UI")]
        [SerializeField] private Button idleButton;
        [SerializeField] private Button walkButton;
        [SerializeField] private Button runButton;
        [SerializeField] private Button jumpButton;
        [SerializeField] private Button stopAnimationButton;
        [SerializeField] private Slider animationSpeedSlider;
        [SerializeField] private Text animationSpeedText;

        [Header("Recording Panel UI")]
        [SerializeField] private Button startRecordingButton;
        [SerializeField] private Button stopRecordingButton;
        [SerializeField] private Button playRecordingButton;
        [SerializeField] private Button exportAnimationButton;
        [SerializeField] private Text recordingStatusText;

        [Header("Settings Panel UI")]
        [SerializeField] private Button portraitButton;
        [SerializeField] private Button landscapeButton;
        [SerializeField] private Dropdown languageDropdown;

        [Header("Navigation")]
        [SerializeField] private Button modelNavButton;
        [SerializeField] private Button filterNavButton;
        [SerializeField] private Button environmentNavButton;
        [SerializeField] private Button boneNavButton;
        [SerializeField] private Button animationNavButton;
        [SerializeField] private Button recordingNavButton;
        [SerializeField] private Button settingsNavButton;

        [Header("References")]
        [SerializeField] private ModelLoader modelLoader;
        [SerializeField] private CameraController cameraController;
        [SerializeField] private FilterManager filterManager;
        [SerializeField] private EnvironmentManager environmentManager;
        [SerializeField] private BoneController boneController;
        [SerializeField] private AnimationController animationController;
        [SerializeField] private RecordingManager recordingManager;
        [SerializeField] private LanguageManager languageManager;

        private GameObject currentPanel;
        private Dictionary<string, Button> navButtonMap;

        private void Awake()
        {
            navButtonMap = new Dictionary<string, Button>();
            InitializeNavigation();
            InitializeControls();
            ShowPanel(modelPanel);
            RegisterLanguageListeners();
        }

        private void Start()
        {
            UpdateUIStrings();
        }

        private void InitializeNavigation()
        {
            if (modelNavButton != null)
                modelNavButton.onClick.AddListener(() => ShowPanel(modelPanel));
            if (filterNavButton != null)
                filterNavButton.onClick.AddListener(() => ShowPanel(filterPanel));
            if (environmentNavButton != null)
                environmentNavButton.onClick.AddListener(() => ShowPanel(environmentPanel));
            if (boneNavButton != null)
                boneNavButton.onClick.AddListener(() => ShowPanel(bonePanel));
            if (animationNavButton != null)
                animationNavButton.onClick.AddListener(() => ShowPanel(animationPanel));
            if (recordingNavButton != null)
                recordingNavButton.onClick.AddListener(() => ShowPanel(recordingPanel));
            if (settingsNavButton != null)
                settingsNavButton.onClick.AddListener(() => ShowPanel(settingsPanel));
        }

        private void InitializeControls()
        {
            if (loadModelButton != null)
                loadModelButton.onClick.AddListener(OnLoadModelClicked);

            if (unloadModelButton != null)
                unloadModelButton.onClick.AddListener(OnUnloadModelClicked);

            if (filterDropdown != null)
                filterDropdown.onValueChanged.AddListener(OnFilterChanged);

            if (filterIntensitySlider != null)
                filterIntensitySlider.onValueChanged.AddListener(OnFilterIntensityChanged);

            if (sunXSlider != null)
                sunXSlider.onValueChanged.AddListener(OnSunXChanged);

            if (sunYSlider != null)
                sunYSlider.onValueChanged.AddListener(OnSunYChanged);

            if (sunIntensitySlider != null)
                sunIntensitySlider.onValueChanged.AddListener(OnSunIntensityChanged);

            if (groundDropdown != null)
                groundDropdown.onValueChanged.AddListener(OnGroundTypeChanged);

            if (addPrimitiveButton != null)
                addPrimitiveButton.onClick.AddListener(OnAddPrimitive);

            if (clearPrimitivesButton != null)
                clearPrimitivesButton.onClick.AddListener(OnClearPrimitives);

            if (boneDropdown != null)
                boneDropdown.onValueChanged.AddListener(OnBoneSelected);

            if (boneRotateXSlider != null)
                boneRotateXSlider.onValueChanged.AddListener(OnBoneRotateChanged);

            if (boneRotateYSlider != null)
                boneRotateYSlider.onValueChanged.AddListener(OnBoneRotateChanged);

            if (boneRotateZSlider != null)
                boneRotateZSlider.onValueChanged.AddListener(OnBoneRotateChanged);

            if (resetBoneButton != null)
                resetBoneButton.onClick.AddListener(() => boneController?.ResetBone());

            if (resetAllBonesButton != null)
                resetAllBonesButton.onClick.AddListener(() => boneController?.ResetAllBones());

            if (idleButton != null)
                idleButton.onClick.AddListener(() => animationController?.PlayIdle());

            if (walkButton != null)
                walkButton.onClick.AddListener(() => animationController?.PlayWalk());

            if (runButton != null)
                runButton.onClick.AddListener(() => animationController?.PlayRun());

            if (jumpButton != null)
                jumpButton.onClick.AddListener(() => animationController?.PlayJump());

            if (stopAnimationButton != null)
                stopAnimationButton.onClick.AddListener(() => animationController?.StopAll());

            if (animationSpeedSlider != null)
                animationSpeedSlider.onValueChanged.AddListener(OnAnimationSpeedChanged);

            if (startRecordingButton != null)
                startRecordingButton.onClick.AddListener(() => recordingManager?.StartRecording());

            if (stopRecordingButton != null)
                stopRecordingButton.onClick.AddListener(() => recordingManager?.StopRecording());

            if (playRecordingButton != null)
                playRecordingButton.onClick.AddListener(() => recordingManager?.PlayAll());

            if (exportAnimationButton != null)
                exportAnimationButton.onClick.AddListener(() => recordingManager?.ExportAnimation(null));

            if (portraitButton != null)
                portraitButton.onClick.AddListener(() => SetOrientation(false));

            if (landscapeButton != null)
                landscapeButton.onClick.AddListener(() => SetOrientation(true));

            if (languageDropdown != null)
                languageDropdown.onValueChanged.AddListener(OnLanguageChanged);
        }

        private void RegisterLanguageListeners()
        {
            if (languageManager != null)
            {
                languageManager.OnLanguageChanged += OnLanguageChangedEvent;
            }
        }

        private void OnLanguageChangedEvent(Language language)
        {
            UpdateUIStrings();
        }

        private void UpdateUIStrings()
        {
            if (languageManager == null) return;

            UpdateButtonText(modelNavButton, "Model");
            UpdateButtonText(filterNavButton, "Filters");
            UpdateButtonText(environmentNavButton, "Environment");
            UpdateButtonText(boneNavButton, "Bones");
            UpdateButtonText(animationNavButton, "Animations");
            UpdateButtonText(recordingNavButton, "Recording");
            UpdateButtonText(settingsNavButton, "Settings");

            UpdateButtonText(loadModelButton, "LoadModel");
            UpdateButtonText(unloadModelButton, "UnloadModel");
            UpdateButtonText(resetBoneButton, "ResetBone");
            UpdateButtonText(resetAllBonesButton, "ResetAllBones");
            UpdateButtonText(idleButton, "Idle");
            UpdateButtonText(walkButton, "Walk");
            UpdateButtonText(runButton, "Run");
            UpdateButtonText(jumpButton, "Jump");
            UpdateButtonText(stopAnimationButton, "StopAnimation");
            UpdateButtonText(startRecordingButton, "StartRecording");
            UpdateButtonText(stopRecordingButton, "StopRecording");
            UpdateButtonText(playRecordingButton, "PlayRecording");
            UpdateButtonText(exportAnimationButton, "ExportAnimation");
            UpdateButtonText(addPrimitiveButton, "AddPrimitive");
            UpdateButtonText(clearPrimitivesButton, "ClearPrimitives");
            UpdateButtonText(portraitButton, "Portrait");
            UpdateButtonText(landscapeButton, "Landscape");
        }

        private void UpdateButtonText(Button button, string key)
        {
            if (button == null) return;
            Text buttonText = button.GetComponentInChildren<Text>();
            if (buttonText != null)
            {
                buttonText.text = languageManager.GetString(key);
            }
        }

        private void ShowPanel(GameObject panel)
        {
            if (currentPanel != null)
            {
                currentPanel.SetActive(false);
            }

            currentPanel = panel;
            if (currentPanel != null)
            {
                currentPanel.SetActive(true);
            }
        }

        private void OnLoadModelClicked()
        {
            if (modelLoader == null) return;

#if UNITY_EDITOR
            string path = UnityEditor.EditorUtility.OpenFilePanel(
                languageManager?.GetString("LoadModel") ?? "Load Model",
                "",
                "fbx,pmx");
            if (!string.IsNullOrEmpty(path))
            {
                GameObject model = modelLoader.LoadModel(path);
                if (model != null && cameraController != null)
                {
                    cameraController.SetTarget(model.transform);

                    if (animationController != null)
                    {
                        animationController.SetAnimator(modelLoader.GetModelAnimator());
                    }

                    if (boneController != null)
                    {
                        boneController.SetAnimator(modelLoader.GetModelAnimator());
                        RefreshBoneDropdown();
                    }

                    UpdateModelStatus(true);
                }
                else
                {
                    UpdateModelStatus(false);
                }
            }
#else
            Debug.LogWarning("[UIManager] File dialog not available at runtime. Use LoadModelFromResources or LoadModelFromBundle.");
#endif
        }

        private void OnUnloadModelClicked()
        {
            modelLoader?.UnloadModel();
            cameraController?.ResetView();
            animationController?.StopAll();
            UpdateModelStatus(null);
            ClearBoneDropdown();
        }

        private void OnFilterChanged(int index)
        {
            if (filterManager == null) return;

            FilterType filterType = index switch
            {
                0 => FilterType.None,
                1 => FilterType.BrightWhite,
                2 => FilterType.SkinBright,
                3 => FilterType.Dark,
                4 => FilterType.GenshinDay,
                5 => FilterType.GenshinNight,
                6 => FilterType.GenshinDusk,
                _ => FilterType.None
            };

            filterManager.ApplyFilter(filterType);
        }

        private void OnFilterIntensityChanged(float value)
        {
            filterManager?.SetIntensity(value);
            if (filterIntensityText != null)
            {
                filterIntensityText.text = $"{value:F2}";
            }
        }

        private void OnSunXChanged(float value)
        {
            if (environmentManager != null)
            {
                Vector3 dir = environmentManager.GetSunDirection();
                dir.x = value;
                environmentManager.SetSunDirection(dir);
            }
        }

        private void OnSunYChanged(float value)
        {
            if (environmentManager != null)
            {
                Vector3 dir = environmentManager.GetSunDirection();
                dir.y = value;
                environmentManager.SetSunDirection(dir);
            }
        }

        private void OnSunIntensityChanged(float value)
        {
            environmentManager?.SetSunIntensity(value);
        }

        private void OnGroundTypeChanged(int index)
        {
            if (environmentManager == null) return;

            GroundType groundType = index switch
            {
                0 => GroundType.Floor,
                1 => GroundType.Grass,
                2 => GroundType.Mud,
                3 => GroundType.Asphalt,
                4 => GroundType.Sky,
                5 => GroundType.Field,
                _ => GroundType.Floor
            };

            environmentManager.SetGroundType(groundType);
        }

        private void OnAddPrimitive()
        {
            if (environmentManager == null || primitiveDropdown == null) return;

            PrimitiveType type = primitiveDropdown.value switch
            {
                0 => PrimitiveType.Cube,
                1 => PrimitiveType.Sphere,
                2 => PrimitiveType.Capsule,
                3 => PrimitiveType.Cylinder,
                _ => PrimitiveType.Cube
            };

            Vector3 position = new Vector3(
                UnityEngine.Random.Range(-3f, 3f),
                0.5f,
                UnityEngine.Random.Range(-3f, 3f));

            environmentManager.AddPrimitive(type, position, Vector3.one,
                new Color(
                    UnityEngine.Random.value,
                    UnityEngine.Random.value,
                    UnityEngine.Random.value));
        }

        private void OnClearPrimitives()
        {
            environmentManager?.ClearPrimitives();
        }

        private void OnBoneSelected(int index)
        {
            if (boneController == null) return;

            List<string> bones = boneController.GetBoneList();
            if (index >= 0 && index < bones.Count)
            {
                boneController.SelectBone(bones[index]);
                if (selectedBoneText != null)
                {
                    selectedBoneText.text = bones[index];
                }
            }
        }

        private void OnBoneRotateChanged(float value)
        {
            if (boneController == null) return;

            float x = boneRotateXSlider != null ? boneRotateXSlider.value : 0f;
            float y = boneRotateYSlider != null ? boneRotateYSlider.value : 0f;
            float z = boneRotateZSlider != null ? boneRotateZSlider.value : 0f;

            boneController.RotateBone(new Vector3(x, y, z));
        }

        private void OnAnimationSpeedChanged(float value)
        {
            animationController?.SetAnimationSpeed(value);
            if (animationSpeedText != null)
            {
                animationSpeedText.text = $"{value:F2}x";
            }
        }

        private void OnLanguageChanged(int index)
        {
            if (languageManager == null) return;

            Language language = index switch
            {
                0 => Language.Chinese,
                1 => Language.English,
                2 => Language.Japanese,
                _ => Language.English
            };

            languageManager.SetLanguage(language);
        }

        private void SetOrientation(bool isLandscape)
        {
            Screen.orientation = isLandscape ? ScreenOrientation.LandscapeLeft : ScreenOrientation.Portrait;
        }

        private void RefreshBoneDropdown()
        {
            if (boneDropdown == null || boneController == null) return;

            boneDropdown.ClearOptions();
            List<string> bones = boneController.GetBoneList();
            boneDropdown.AddOptions(bones);

            if (boneDropdown.value >= 0 && boneDropdown.value < bones.Count)
            {
                OnBoneSelected(boneDropdown.value);
            }
        }

        private void ClearBoneDropdown()
        {
            if (boneDropdown != null)
            {
                boneDropdown.ClearOptions();
            }
            if (selectedBoneText != null)
            {
                selectedBoneText.text = "";
            }
        }

        private void UpdateModelStatus(bool? success)
        {
            if (modelStatusText == null || languageManager == null) return;

            if (success == true)
            {
                modelStatusText.text = languageManager.GetString("ModelLoaded");
                modelStatusText.color = Color.green;
            }
            else if (success == false)
            {
                modelStatusText.text = languageManager.GetString("ModelFailed");
                modelStatusText.color = Color.red;
            }
            else
            {
                modelStatusText.text = languageManager.GetString("NoModelLoaded");
                modelStatusText.color = Color.gray;
            }
        }

        public void ShowNotification(string message, Color color)
        {
            if (modelStatusText != null)
            {
                modelStatusText.text = message;
                modelStatusText.color = color;
            }
        }

        private void OnDestroy()
        {
            if (languageManager != null)
            {
                languageManager.OnLanguageChanged -= OnLanguageChangedEvent;
            }
        }
    }
}