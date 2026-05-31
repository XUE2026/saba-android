using System.Collections.Generic;
using UnityEngine;

namespace UnitySabaViewer
{
    public class LanguageManager : MonoBehaviour
    {
        private static LanguageManager _instance;
        public static LanguageManager Instance
        {
            get
            {
                if (_instance == null)
                {
                    _instance = FindObjectOfType<LanguageManager>();
                    if (_instance == null)
                    {
                        GameObject go = new GameObject("LanguageManager");
                        _instance = go.AddComponent<LanguageManager>();
                        DontDestroyOnLoad(go);
                    }
                }
                return _instance;
            }
        }

        public enum Language
        {
            Chinese = 0,
            English = 1,
            Japanese = 2
        }

        private Language _currentLanguage = Language.Chinese;
        private Dictionary<string, string[]> _strings;

        public Language CurrentLanguage => _currentLanguage;

        private void Awake()
        {
            if (_instance != null && _instance != this)
            {
                Destroy(gameObject);
                return;
            }

            _instance = this;
            DontDestroyOnLoad(gameObject);

            InitializeStrings();

            int savedLang = PlayerPrefs.GetInt("SabaViewer_Language", 0);
            _currentLanguage = (Language)Mathf.Clamp(savedLang, 0, 2);
        }

        private void InitializeStrings()
        {
            _strings = new Dictionary<string, string[]>();

            _strings["AppName"] = new string[] { "SabaViewer", "SabaViewer", "SabaViewer" };
            _strings["Model"] = new string[] { "模型", "Model", "モデル" };
            _strings["Filter"] = new string[] { "滤镜", "Filter", "フィルター" };
            _strings["Environment"] = new string[] { "环境", "Environment", "環境" };
            _strings["Bone"] = new string[] { "骨骼", "Bone", "ボーン" };
            _strings["Animation"] = new string[] { "动画", "Animation", "アニメーション" };
            _strings["Recording"] = new string[] { "录制", "Recording", "録画" };
            _strings["Settings"] = new string[] { "设置", "Settings", "設定" };
            _strings["Language"] = new string[] { "语言", "Language", "言語" };

            _strings["LoadModel"] = new string[] { "加载模型", "Load Model", "モデルを読み込む" };
            _strings["UnloadModel"] = new string[] { "卸载模型", "Unload Model", "モデルをアンロード" };
            _strings["ModelStatus"] = new string[] { "模型状态", "Model Status", "モデル状態" };
            _strings["NoModelLoaded"] = new string[] { "未加载模型", "No Model Loaded", "モデル未読み込み" };
            _strings["ModelLoaded"] = new string[] { "模型已加载", "Model Loaded", "モデル読み込み済み" };
            _strings["SelectModelFile"] = new string[] { "选择模型文件", "Select Model File", "モデルファイルを選択" };

            _strings["FilterPreset"] = new string[] { "滤镜预设", "Filter Preset", "フィルタープリセット" };
            _strings["Exposure"] = new string[] { "曝光", "Exposure", "露出" };
            _strings["Contrast"] = new string[] { "对比度", "Contrast", "コントラスト" };
            _strings["Saturation"] = new string[] { "饱和度", "Saturation", "彩度" };
            _strings["HueShift"] = new string[] { "色相偏移", "Hue Shift", "色相シフト" };
            _strings["Temperature"] = new string[] { "色温", "Temperature", "色温度" };
            _strings["Tint"] = new string[] { "色调", "Tint", "色合い" };

            _strings["SunLight"] = new string[] { "太阳光", "Sun Light", "太陽光" };
            _strings["SunIntensity"] = new string[] { "光照强度", "Sun Intensity", "光の強さ" };
            _strings["SunDirection"] = new string[] { "光照方向", "Sun Direction", "光の方向" };
            _strings["GroundType"] = new string[] { "地面类型", "Ground Type", "地面タイプ" };
            _strings["Grid"] = new string[] { "网格", "Grid", "グリッド" };
            _strings["Checker"] = new string[] { "棋盘格", "Checker", "チェッカー" };
            _strings["Grass"] = new string[] { "草地", "Grass", "芝生" };
            _strings["Dirt"] = new string[] { "泥土", "Dirt", "土" };
            _strings["Asphalt"] = new string[] { "沥青", "Asphalt", "アスファルト" };
            _strings["AddPrimitive"] = new string[] { "添加基本体", "Add Primitive", "プリミティブを追加" };
            _strings["Cube"] = new string[] { "立方体", "Cube", "立方体" };
            _strings["Sphere"] = new string[] { "球体", "Sphere", "球体" };
            _strings["Cone"] = new string[] { "圆锥", "Cone", "円錐" };
            _strings["Cylinder"] = new string[] { "圆柱", "Cylinder", "円柱" };

            _strings["BoneList"] = new string[] { "骨骼列表", "Bone List", "ボーン一覧" };
            _strings["RotateBone"] = new string[] { "旋转骨骼", "Rotate Bone", "ボーンを回転" };
            _strings["TranslateBone"] = new string[] { "位移骨骼", "Translate Bone", "ボーンを移動" };
            _strings["ResetBone"] = new string[] { "重置骨骼", "Reset Bone", "ボーンをリセット" };
            _strings["ResetAllBones"] = new string[] { "重置所有骨骼", "Reset All Bones", "全ボーンをリセット" };

            _strings["PlayIdle"] = new string[] { "待机", "Idle", "待機" };
            _strings["PlayWalk"] = new string[] { "行走", "Walk", "歩行" };
            _strings["PlayRun"] = new string[] { "奔跑", "Run", "走る" };
            _strings["PlayJump"] = new string[] { "跳跃", "Jump", "ジャンプ" };
            _strings["StopAnimation"] = new string[] { "停止", "Stop", "停止" };
            _strings["CrossFade"] = new string[] { "过渡", "CrossFade", "クロスフェード" };

            _strings["StartRecording"] = new string[] { "开始录制", "Start Recording", "録画開始" };
            _strings["StopRecording"] = new string[] { "停止录制", "Stop Recording", "録画停止" };
            _strings["ExportVMD"] = new string[] { "导出VMD", "Export VMD", "VMD出力" };
            _strings["ExportGobotAction"] = new string[] { "导出GoBot动作", "Export GoBot Action", "GoBotアクション出力" };
            _strings["RecordingInProgress"] = new string[] { "录制中...", "Recording...", "録画中..." };

            _strings["CameraSensitivity"] = new string[] { "相机灵敏度", "Camera Sensitivity", "カメラ感度" };
            _strings["VSync"] = new string[] { "垂直同步", "VSync", "垂直同期" };
            _strings["Quality"] = new string[] { "画质", "Quality", "画質" };
            _strings["ResetCamera"] = new string[] { "重置相机", "Reset Camera", "カメラをリセット" };

            _strings["Chinese"] = new string[] { "中文", "Chinese", "中国語" };
            _strings["English"] = new string[] { "英文", "English", "英語" };
            _strings["Japanese"] = new string[] { "日文", "Japanese", "日本語" };

            _strings["Confirm"] = new string[] { "确认", "Confirm", "確認" };
            _strings["Cancel"] = new string[] { "取消", "Cancel", "キャンセル" };
            _strings["OK"] = new string[] { "确定", "OK", "OK" };
            _strings["Close"] = new string[] { "关闭", "Close", "閉じる" };
            _strings["Error"] = new string[] { "错误", "Error", "エラー" };
            _strings["Warning"] = new string[] { "警告", "Warning", "警告" };
            _strings["Success"] = new string[] { "成功", "Success", "成功" };
            _strings["Failed"] = new string[] { "失败", "Failed", "失敗" };
            _strings["Loading"] = new string[] { "加载中...", "Loading...", "読み込み中..." };
            _strings["Save"] = new string[] { "保存", "Save", "保存" };
        }

        public void SetLanguage(int lang)
        {
            if (lang < 0 || lang > 2)
            {
                Debug.LogWarning($"[LanguageManager] Invalid language index: {lang}");
                return;
            }

            _currentLanguage = (Language)lang;
            PlayerPrefs.SetInt("SabaViewer_Language", lang);
            PlayerPrefs.Save();

            Debug.Log($"[LanguageManager] Language set to: {_currentLanguage}");
        }

        public void SetLanguage(Language lang)
        {
            SetLanguage((int)lang);
        }

        public string GetString(string key)
        {
            if (_strings == null)
            {
                InitializeStrings();
            }

            if (_strings.TryGetValue(key, out string[] translations))
            {
                int index = (int)_currentLanguage;
                if (index >= 0 && index < translations.Length)
                {
                    return translations[index];
                }
                return translations[0];
            }

            Debug.LogWarning($"[LanguageManager] Missing string key: {key}");
            return key;
        }

        public bool HasKey(string key)
        {
            return _strings != null && _strings.ContainsKey(key);
        }

        public string[] GetAllKeys()
        {
            if (_strings == null) return new string[0];
            string[] keys = new string[_strings.Count];
            _strings.Keys.CopyTo(keys, 0);
            return keys;
        }

        public int GetLanguageIndex()
        {
            return (int)_currentLanguage;
        }

        public string GetLanguageName()
        {
            switch (_currentLanguage)
            {
                case Language.Chinese: return "中文";
                case Language.English: return "English";
                case Language.Japanese: return "日本語";
                default: return "Unknown";
            }
        }
    }
}