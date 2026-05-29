using System;
using System.Collections.Generic;
using UnityEngine;

namespace Saba.MMDViewer
{
    public enum Language
    {
        Chinese,
        English,
        Japanese
    }

    public class LanguageManager : MonoBehaviour
    {
        public static LanguageManager Instance { get; private set; }

        [Header("Language Settings")]
        [SerializeField] private Language currentLanguage = Language.Chinese;

        private Dictionary<string, string> chineseStrings = new Dictionary<string, string>();
        private Dictionary<string, string> englishStrings = new Dictionary<string, string>();
        private Dictionary<string, string> japaneseStrings = new Dictionary<string, string>();

        public event Action<Language> OnLanguageChanged;

        private void Awake()
        {
            if (Instance != null && Instance != this)
            {
                Destroy(gameObject);
                return;
            }

            Instance = this;
            DontDestroyOnLoad(gameObject);

            InitializeTranslations();
        }

        private void InitializeTranslations()
        {
            chineseStrings["Model"] = "模型";
            chineseStrings["Filters"] = "滤镜";
            chineseStrings["Environment"] = "环境";
            chineseStrings["Bones"] = "骨骼";
            chineseStrings["Animations"] = "动画";
            chineseStrings["Recording"] = "录制";
            chineseStrings["Settings"] = "设置";
            chineseStrings["LoadModel"] = "加载模型";
            chineseStrings["UnloadModel"] = "卸载模型";
            chineseStrings["ResetView"] = "重置视角";
            chineseStrings["ApplyFilter"] = "应用滤镜";
            chineseStrings["Intensity"] = "强度";
            chineseStrings["SunDirection"] = "太阳方向";
            chineseStrings["SunIntensity"] = "太阳强度";
            chineseStrings["GroundType"] = "地面类型";
            chineseStrings["SelectBone"] = "选择骨骼";
            chineseStrings["RotateBone"] = "旋转骨骼";
            chineseStrings["ResetBone"] = "重置骨骼";
            chineseStrings["ResetAllBones"] = "重置所有骨骼";
            chineseStrings["PlayAnimation"] = "播放动画";
            chineseStrings["StopAnimation"] = "停止动画";
            chineseStrings["AnimationSpeed"] = "动画速度";
            chineseStrings["StartRecording"] = "开始录制";
            chineseStrings["StopRecording"] = "停止录制";
            chineseStrings["PlayRecording"] = "播放录制";
            chineseStrings["ExportAnimation"] = "导出动画";
            chineseStrings["Orientation"] = "方向";
            chineseStrings["Portrait"] = "竖屏";
            chineseStrings["Landscape"] = "横屏";
            chineseStrings["Language"] = "语言";
            chineseStrings["None"] = "无";
            chineseStrings["Floor"] = "地板";
            chineseStrings["Grass"] = "草地";
            chineseStrings["Mud"] = "泥地";
            chineseStrings["Asphalt"] = "沥青";
            chineseStrings["Sky"] = "天空";
            chineseStrings["Field"] = "田野";
            chineseStrings["Idle"] = "待机";
            chineseStrings["Walk"] = "行走";
            chineseStrings["Run"] = "奔跑";
            chineseStrings["Jump"] = "跳跃";
            chineseStrings["BrightWhite"] = "亮白";
            chineseStrings["SkinBright"] = "亮肤";
            chineseStrings["Dark"] = "暗黑";
            chineseStrings["GenshinDay"] = "原神白天";
            chineseStrings["GenshinNight"] = "原神夜晚";
            chineseStrings["GenshinDusk"] = "原神黄昏";
            chineseStrings["AddPrimitive"] = "添加基本体";
            chineseStrings["ClearPrimitives"] = "清除基本体";
            chineseStrings["Cube"] = "立方体";
            chineseStrings["Sphere"] = "球体";
            chineseStrings["Capsule"] = "胶囊体";
            chineseStrings["Cylinder"] = "圆柱体";
            chineseStrings["SegmentGap"] = "片段间隔";
            chineseStrings["NoModelLoaded"] = "未加载模型";
            chineseStrings["LoadingModel"] = "正在加载模型...";
            chineseStrings["ModelLoaded"] = "模型加载成功";
            chineseStrings["ModelFailed"] = "模型加载失败";
            chineseStrings["GoBotImport"] = "导入GoBot";
            chineseStrings["GoBotExport"] = "导出GoBot";
            chineseStrings["Cancel"] = "取消";
            chineseStrings["Confirm"] = "确认";

            englishStrings["Model"] = "Model";
            englishStrings["Filters"] = "Filters";
            englishStrings["Environment"] = "Environment";
            englishStrings["Bones"] = "Bones";
            englishStrings["Animations"] = "Animations";
            englishStrings["Recording"] = "Recording";
            englishStrings["Settings"] = "Settings";
            englishStrings["LoadModel"] = "Load Model";
            englishStrings["UnloadModel"] = "Unload Model";
            englishStrings["ResetView"] = "Reset View";
            englishStrings["ApplyFilter"] = "Apply Filter";
            englishStrings["Intensity"] = "Intensity";
            englishStrings["SunDirection"] = "Sun Direction";
            englishStrings["SunIntensity"] = "Sun Intensity";
            englishStrings["GroundType"] = "Ground Type";
            englishStrings["SelectBone"] = "Select Bone";
            englishStrings["RotateBone"] = "Rotate Bone";
            englishStrings["ResetBone"] = "Reset Bone";
            englishStrings["ResetAllBones"] = "Reset All Bones";
            englishStrings["PlayAnimation"] = "Play Animation";
            englishStrings["StopAnimation"] = "Stop Animation";
            englishStrings["AnimationSpeed"] = "Animation Speed";
            englishStrings["StartRecording"] = "Start Recording";
            englishStrings["StopRecording"] = "Stop Recording";
            englishStrings["PlayRecording"] = "Play Recording";
            englishStrings["ExportAnimation"] = "Export Animation";
            englishStrings["Orientation"] = "Orientation";
            englishStrings["Portrait"] = "Portrait";
            englishStrings["Landscape"] = "Landscape";
            englishStrings["Language"] = "Language";
            englishStrings["None"] = "None";
            englishStrings["Floor"] = "Floor";
            englishStrings["Grass"] = "Grass";
            englishStrings["Mud"] = "Mud";
            englishStrings["Asphalt"] = "Asphalt";
            englishStrings["Sky"] = "Sky";
            englishStrings["Field"] = "Field";
            englishStrings["Idle"] = "Idle";
            englishStrings["Walk"] = "Walk";
            englishStrings["Run"] = "Run";
            englishStrings["Jump"] = "Jump";
            englishStrings["BrightWhite"] = "Bright White";
            englishStrings["SkinBright"] = "Skin Bright";
            englishStrings["Dark"] = "Dark";
            englishStrings["GenshinDay"] = "Genshin Day";
            englishStrings["GenshinNight"] = "Genshin Night";
            englishStrings["GenshinDusk"] = "Genshin Dusk";
            englishStrings["AddPrimitive"] = "Add Primitive";
            englishStrings["ClearPrimitives"] = "Clear Primitives";
            englishStrings["Cube"] = "Cube";
            englishStrings["Sphere"] = "Sphere";
            englishStrings["Capsule"] = "Capsule";
            englishStrings["Cylinder"] = "Cylinder";
            englishStrings["SegmentGap"] = "Segment Gap";
            englishStrings["NoModelLoaded"] = "No Model Loaded";
            englishStrings["LoadingModel"] = "Loading Model...";
            englishStrings["ModelLoaded"] = "Model Loaded Successfully";
            englishStrings["ModelFailed"] = "Model Load Failed";
            englishStrings["GoBotImport"] = "Import GoBot";
            englishStrings["GoBotExport"] = "Export GoBot";
            englishStrings["Cancel"] = "Cancel";
            englishStrings["Confirm"] = "Confirm";

            japaneseStrings["Model"] = "モデル";
            japaneseStrings["Filters"] = "フィルター";
            japaneseStrings["Environment"] = "環境";
            japaneseStrings["Bones"] = "ボーン";
            japaneseStrings["Animations"] = "アニメーション";
            japaneseStrings["Recording"] = "録画";
            japaneseStrings["Settings"] = "設定";
            japaneseStrings["LoadModel"] = "モデルを読み込む";
            japaneseStrings["UnloadModel"] = "モデルをアンロード";
            japaneseStrings["ResetView"] = "ビューをリセット";
            japaneseStrings["ApplyFilter"] = "フィルターを適用";
            japaneseStrings["Intensity"] = "強度";
            japaneseStrings["SunDirection"] = "太陽の方向";
            japaneseStrings["SunIntensity"] = "太陽の強さ";
            japaneseStrings["GroundType"] = "地面の種類";
            japaneseStrings["SelectBone"] = "ボーンを選択";
            japaneseStrings["RotateBone"] = "ボーンを回転";
            japaneseStrings["ResetBone"] = "ボーンをリセット";
            japaneseStrings["ResetAllBones"] = "すべてのボーンをリセット";
            japaneseStrings["PlayAnimation"] = "アニメーション再生";
            japaneseStrings["StopAnimation"] = "アニメーション停止";
            japaneseStrings["AnimationSpeed"] = "アニメーション速度";
            japaneseStrings["StartRecording"] = "録画開始";
            japaneseStrings["StopRecording"] = "録画停止";
            japaneseStrings["PlayRecording"] = "録画再生";
            japaneseStrings["ExportAnimation"] = "アニメーションをエクスポート";
            japaneseStrings["Orientation"] = "画面向き";
            japaneseStrings["Portrait"] = "縦向き";
            japaneseStrings["Landscape"] = "横向き";
            japaneseStrings["Language"] = "言語";
            japaneseStrings["None"] = "なし";
            japaneseStrings["Floor"] = "フロア";
            japaneseStrings["Grass"] = "草";
            japaneseStrings["Mud"] = "泥";
            japaneseStrings["Asphalt"] = "アスファルト";
            japaneseStrings["Sky"] = "空";
            japaneseStrings["Field"] = "フィールド";
            japaneseStrings["Idle"] = "待機";
            japaneseStrings["Walk"] = "歩く";
            japaneseStrings["Run"] = "走る";
            japaneseStrings["Jump"] = "ジャンプ";
            japaneseStrings["BrightWhite"] = "ブライトホワイト";
            japaneseStrings["SkinBright"] = "スキンブライト";
            japaneseStrings["Dark"] = "ダーク";
            japaneseStrings["GenshinDay"] = "原神昼";
            japaneseStrings["GenshinNight"] = "原神夜";
            japaneseStrings["GenshinDusk"] = "原神黄昏";
            japaneseStrings["AddPrimitive"] = "プリミティブ追加";
            japaneseStrings["ClearPrimitives"] = "プリミティブ削除";
            japaneseStrings["Cube"] = "キューブ";
            japaneseStrings["Sphere"] = "スフィア";
            japaneseStrings["Capsule"] = "カプセル";
            japaneseStrings["Cylinder"] = "シリンダー";
            japaneseStrings["SegmentGap"] = "セグメント間隔";
            japaneseStrings["NoModelLoaded"] = "モデルが読み込まれていません";
            japaneseStrings["LoadingModel"] = "モデルを読み込んでいます...";
            japaneseStrings["ModelLoaded"] = "モデルの読み込みに成功しました";
            japaneseStrings["ModelFailed"] = "モデルの読み込みに失敗しました";
            japaneseStrings["GoBotImport"] = "GoBotをインポート";
            japaneseStrings["GoBotExport"] = "GoBotをエクスポート";
            japaneseStrings["Cancel"] = "キャンセル";
            japaneseStrings["Confirm"] = "確認";
        }

        public void SetLanguage(Language language)
        {
            if (currentLanguage == language) return;

            currentLanguage = language;
            OnLanguageChanged?.Invoke(currentLanguage);
        }

        public Language GetCurrentLanguage()
        {
            return currentLanguage;
        }

        public string GetString(string key)
        {
            Dictionary<string, string> dictionary = currentLanguage switch
            {
                Language.Chinese => chineseStrings,
                Language.English => englishStrings,
                Language.Japanese => japaneseStrings,
                _ => englishStrings
            };

            if (dictionary.TryGetValue(key, out string value))
            {
                return value;
            }

            if (englishStrings.TryGetValue(key, out string fallback))
            {
                return fallback;
            }

            return $"[{key}]";
        }

        public bool HasKey(string key)
        {
            Dictionary<string, string> dictionary = currentLanguage switch
            {
                Language.Chinese => chineseStrings,
                Language.English => englishStrings,
                Language.Japanese => japaneseStrings,
                _ => englishStrings
            };

            return dictionary.ContainsKey(key);
        }

        public string GetStringInLanguage(string key, Language language)
        {
            Dictionary<string, string> dictionary = language switch
            {
                Language.Chinese => chineseStrings,
                Language.English => englishStrings,
                Language.Japanese => japaneseStrings,
                _ => englishStrings
            };

            if (dictionary.TryGetValue(key, out string value))
            {
                return value;
            }

            if (englishStrings.TryGetValue(key, out string fallback))
            {
                return fallback;
            }

            return $"[{key}]";
        }
    }
}