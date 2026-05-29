//
// Copyright(c) 2016-2017 benikabocha.
// Copyright(c) 2026 xueyixuan2026.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "LocalizationManager.h"

namespace saba
{
    LocalizationManager& LocalizationManager::GetInstance()
    {
        static LocalizationManager instance;
        return instance;
    }

    LocalizationManager::LocalizationManager()
        : m_currentLanguage(Language::English)
    {
        Initialize();
    }

    void LocalizationManager::SetLanguage(Language lang)
    {
        m_currentLanguage = lang;
    }

    LocalizationManager::Language LocalizationManager::GetLanguage() const
    {
        return m_currentLanguage;
    }

    const std::string& LocalizationManager::GetString(const std::string& key) const
    {
        static const std::string s_empty;

        auto langIt = m_translations.find(m_currentLanguage);
        if (langIt != m_translations.end())
        {
            auto strIt = langIt->second.find(key);
            if (strIt != langIt->second.end())
            {
                return strIt->second;
            }
        }

        auto engIt = m_translations.find(Language::English);
        if (engIt != m_translations.end())
        {
            auto strIt = engIt->second.find(key);
            if (strIt != engIt->second.end())
            {
                return strIt->second;
            }
        }

        return s_empty;
    }

    void LocalizationManager::Initialize()
    {
        std::map<std::string, std::string> en;
        std::map<std::string, std::string> zh;
        std::map<std::string, std::string> ja;

        // Model
        en["Model.LoadModel"] = "Load Model";
        zh["Model.LoadModel"] = "加载模型";
        ja["Model.LoadModel"] = "モデルを読み込む";

        en["Model.Unload"] = "Unload";
        zh["Model.Unload"] = "卸载";
        ja["Model.Unload"] = "アンロード";

        en["Model.Materials"] = "Materials";
        zh["Model.Materials"] = "材质";
        ja["Model.Materials"] = "マテリアル";

        en["Model.Textures"] = "Textures";
        zh["Model.Textures"] = "纹理";
        ja["Model.Textures"] = "テクスチャ";

        en["Model.Clothing"] = "Clothing";
        zh["Model.Clothing"] = "服装";
        ja["Model.Clothing"] = "衣装";

        // Filters
        en["Filters.Filters"] = "Filters";
        zh["Filters.Filters"] = "滤镜";
        ja["Filters.Filters"] = "フィルター";

        en["Filters.None"] = "None";
        zh["Filters.None"] = "无";
        ja["Filters.None"] = "なし";

        en["Filters.BrightWhite"] = "Bright White";
        zh["Filters.BrightWhite"] = "亮白";
        ja["Filters.BrightWhite"] = "ブライトホワイト";

        en["Filters.SkinBright"] = "Skin Bright";
        zh["Filters.SkinBright"] = "肤色提亮";
        ja["Filters.SkinBright"] = "スキンブライト";

        en["Filters.Dark"] = "Dark";
        zh["Filters.Dark"] = "暗色调";
        ja["Filters.Dark"] = "ダーク";

        en["Filters.GenshinDay"] = "Genshin Day";
        zh["Filters.GenshinDay"] = "原神白天";
        ja["Filters.GenshinDay"] = "原神デイ";

        en["Filters.GenshinNight"] = "Genshin Night";
        zh["Filters.GenshinNight"] = "原神夜晚";
        ja["Filters.GenshinNight"] = "原神ナイト";

        en["Filters.GenshinDusk"] = "Genshin Dusk";
        zh["Filters.GenshinDusk"] = "原神黄昏";
        ja["Filters.GenshinDusk"] = "原神ダスク";

        en["Filters.Custom"] = "Custom";
        zh["Filters.Custom"] = "自定义";
        ja["Filters.Custom"] = "カスタム";

        en["Filters.Intensity"] = "Intensity";
        zh["Filters.Intensity"] = "强度";
        ja["Filters.Intensity"] = "強度";

        en["Filters.Exposure"] = "Exposure";
        zh["Filters.Exposure"] = "曝光";
        ja["Filters.Exposure"] = "露出";

        en["Filters.Contrast"] = "Contrast";
        zh["Filters.Contrast"] = "对比度";
        ja["Filters.Contrast"] = "コントラスト";

        en["Filters.Saturation"] = "Saturation";
        zh["Filters.Saturation"] = "饱和度";
        ja["Filters.Saturation"] = "彩度";

        en["Filters.HueShift"] = "Hue Shift";
        zh["Filters.HueShift"] = "色相偏移";
        ja["Filters.HueShift"] = "色相シフト";

        en["Filters.Temperature"] = "Temperature";
        zh["Filters.Temperature"] = "色温";
        ja["Filters.Temperature"] = "色温度";

        en["Filters.Tint"] = "Tint";
        zh["Filters.Tint"] = "色调";
        ja["Filters.Tint"] = "ティント";

        en["Filters.ResetDefaults"] = "Reset Defaults";
        zh["Filters.ResetDefaults"] = "重置默认";
        ja["Filters.ResetDefaults"] = "デフォルトにリセット";

        // Environment
        en["Environment.Environment"] = "Environment";
        zh["Environment.Environment"] = "环境";
        ja["Environment.Environment"] = "環境";

        en["Environment.Sun"] = "Sun";
        zh["Environment.Sun"] = "太阳";
        ja["Environment.Sun"] = "太陽";

        en["Environment.DirectionX"] = "Direction X";
        zh["Environment.DirectionX"] = "方向 X";
        ja["Environment.DirectionX"] = "方向 X";

        en["Environment.DirectionY"] = "Direction Y";
        zh["Environment.DirectionY"] = "方向 Y";
        ja["Environment.DirectionY"] = "方向 Y";

        en["Environment.DirectionZ"] = "Direction Z";
        zh["Environment.DirectionZ"] = "方向 Z";
        ja["Environment.DirectionZ"] = "方向 Z";

        en["Environment.Intensity"] = "Intensity";
        zh["Environment.Intensity"] = "强度";
        ja["Environment.Intensity"] = "強度";

        en["Environment.Color"] = "Color";
        zh["Environment.Color"] = "颜色";
        ja["Environment.Color"] = "色";

        en["Environment.Ground"] = "Ground";
        zh["Environment.Ground"] = "地面";
        ja["Environment.Ground"] = "地面";

        en["Environment.Floor"] = "Floor";
        zh["Environment.Floor"] = "地板";
        ja["Environment.Floor"] = "フロア";

        en["Environment.Grass"] = "Grass";
        zh["Environment.Grass"] = "草地";
        ja["Environment.Grass"] = "芝生";

        en["Environment.Mud"] = "Mud";
        zh["Environment.Mud"] = "泥地";
        ja["Environment.Mud"] = "泥";

        en["Environment.Asphalt"] = "Asphalt";
        zh["Environment.Asphalt"] = "沥青";
        ja["Environment.Asphalt"] = "アスファルト";

        en["Environment.Sky"] = "Sky";
        zh["Environment.Sky"] = "天空";
        ja["Environment.Sky"] = "空";

        en["Environment.Field"] = "Field";
        zh["Environment.Field"] = "场地";
        ja["Environment.Field"] = "フィールド";

        en["Environment.Primitives"] = "Primitives";
        zh["Environment.Primitives"] = "基本几何体";
        ja["Environment.Primitives"] = "プリミティブ";

        en["Environment.Cube"] = "Cube";
        zh["Environment.Cube"] = "立方体";
        ja["Environment.Cube"] = "キューブ";

        en["Environment.Sphere"] = "Sphere";
        zh["Environment.Sphere"] = "球体";
        ja["Environment.Sphere"] = "球";

        en["Environment.Cone"] = "Cone";
        zh["Environment.Cone"] = "圆锥";
        ja["Environment.Cone"] = "円錐";

        en["Environment.Cylinder"] = "Cylinder";
        zh["Environment.Cylinder"] = "圆柱";
        ja["Environment.Cylinder"] = "円柱";

        en["Environment.Add"] = "Add";
        zh["Environment.Add"] = "添加";
        ja["Environment.Add"] = "追加";

        en["Environment.ClearAll"] = "Clear All";
        zh["Environment.ClearAll"] = "清除全部";
        ja["Environment.ClearAll"] = "すべてクリア";

        en["Environment.Delete"] = "Delete";
        zh["Environment.Delete"] = "删除";
        ja["Environment.Delete"] = "削除";

        // Bones
        en["Bones.Bones"] = "Bones";
        zh["Bones.Bones"] = "骨骼";
        ja["Bones.Bones"] = "ボーン";

        en["Bones.SelectBone"] = "Select Bone";
        zh["Bones.SelectBone"] = "选择骨骼";
        ja["Bones.SelectBone"] = "ボーン選択";

        en["Bones.RotationX"] = "Rotation X";
        zh["Bones.RotationX"] = "旋转 X";
        ja["Bones.RotationX"] = "回転 X";

        en["Bones.RotationY"] = "Rotation Y";
        zh["Bones.RotationY"] = "旋转 Y";
        ja["Bones.RotationY"] = "回転 Y";

        en["Bones.RotationZ"] = "Rotation Z";
        zh["Bones.RotationZ"] = "旋转 Z";
        ja["Bones.RotationZ"] = "回転 Z";

        en["Bones.TranslationX"] = "Translation X";
        zh["Bones.TranslationX"] = "平移 X";
        ja["Bones.TranslationX"] = "移動 X";

        en["Bones.TranslationY"] = "Translation Y";
        zh["Bones.TranslationY"] = "平移 Y";
        ja["Bones.TranslationY"] = "移動 Y";

        en["Bones.TranslationZ"] = "Translation Z";
        zh["Bones.TranslationZ"] = "平移 Z";
        ja["Bones.TranslationZ"] = "移動 Z";

        en["Bones.Apply"] = "Apply";
        zh["Bones.Apply"] = "应用";
        ja["Bones.Apply"] = "適用";

        en["Bones.ResetBone"] = "Reset Bone";
        zh["Bones.ResetBone"] = "重置骨骼";
        ja["Bones.ResetBone"] = "ボーンリセット";

        en["Bones.ResetAll"] = "Reset All";
        zh["Bones.ResetAll"] = "重置全部";
        ja["Bones.ResetAll"] = "すべてリセット";

        en["Bones.BuiltinAnimations"] = "Built-in Animations";
        zh["Bones.BuiltinAnimations"] = "内置动画";
        ja["Bones.BuiltinAnimations"] = "内蔵アニメーション";

        en["Bones.Idle"] = "Idle";
        zh["Bones.Idle"] = "待机";
        ja["Bones.Idle"] = "アイドル";

        en["Bones.Walk"] = "Walk";
        zh["Bones.Walk"] = "行走";
        ja["Bones.Walk"] = "歩く";

        en["Bones.Run"] = "Run";
        zh["Bones.Run"] = "跑步";
        ja["Bones.Run"] = "走る";

        en["Bones.Jump"] = "Jump";
        zh["Bones.Jump"] = "跳跃";
        ja["Bones.Jump"] = "ジャンプ";

        en["Bones.StopAnimation"] = "Stop Animation";
        zh["Bones.StopAnimation"] = "停止动画";
        ja["Bones.StopAnimation"] = "アニメーション停止";

        // Recording
        en["Recording.Recording"] = "Recording";
        zh["Recording.Recording"] = "录制";
        ja["Recording.Recording"] = "録画";

        en["Recording.StartRecording"] = "Start Recording";
        zh["Recording.StartRecording"] = "开始录制";
        ja["Recording.StartRecording"] = "録画開始";

        en["Recording.StopRecording"] = "Stop Recording";
        zh["Recording.StopRecording"] = "停止录制";
        ja["Recording.StopRecording"] = "録画停止";

        en["Recording.Save"] = "Save";
        zh["Recording.Save"] = "保存";
        ja["Recording.Save"] = "保存";

        en["Recording.Load"] = "Load";
        zh["Recording.Load"] = "加载";
        ja["Recording.Load"] = "読み込む";

        en["Recording.AutoSplit"] = "Auto Split";
        zh["Recording.AutoSplit"] = "自动分割";
        ja["Recording.AutoSplit"] = "自動分割";

        en["Recording.GapTime"] = "Gap Time";
        zh["Recording.GapTime"] = "间隔时间";
        ja["Recording.GapTime"] = "ギャップ時間";

        en["Recording.MergeAll"] = "Merge All";
        zh["Recording.MergeAll"] = "合并全部";
        ja["Recording.MergeAll"] = "すべて結合";

        en["Recording.ExportVMD"] = "Export VMD";
        zh["Recording.ExportVMD"] = "导出 VMD";
        ja["Recording.ExportVMD"] = "VMD エクスポート";

        en["Recording.Play"] = "Play";
        zh["Recording.Play"] = "播放";
        ja["Recording.Play"] = "再生";

        en["Recording.Pause"] = "Pause";
        zh["Recording.Pause"] = "暂停";
        ja["Recording.Pause"] = "一時停止";

        en["Recording.Resume"] = "Resume";
        zh["Recording.Resume"] = "继续";
        ja["Recording.Resume"] = "再開";

        en["Recording.Stop"] = "Stop";
        zh["Recording.Stop"] = "停止";
        ja["Recording.Stop"] = "停止";

        // Settings
        en["Settings.Settings"] = "Settings";
        zh["Settings.Settings"] = "设置";
        ja["Settings.Settings"] = "設定";

        en["Settings.Orientation"] = "Orientation";
        zh["Settings.Orientation"] = "屏幕方向";
        ja["Settings.Orientation"] = "画面の向き";

        en["Settings.Portrait"] = "Portrait";
        zh["Settings.Portrait"] = "竖屏";
        ja["Settings.Portrait"] = "縦画面";

        en["Settings.Landscape"] = "Landscape";
        zh["Settings.Landscape"] = "横屏";
        ja["Settings.Landscape"] = "横画面";

        en["Settings.Language"] = "Language";
        zh["Settings.Language"] = "语言";
        ja["Settings.Language"] = "言語";

        en["Settings.About"] = "About";
        zh["Settings.About"] = "关于";
        ja["Settings.About"] = "情報";

        en["Settings.Version"] = "Version";
        zh["Settings.Version"] = "版本";
        ja["Settings.Version"] = "バージョン";

        // General
        en["General.Open"] = "Open";
        zh["General.Open"] = "打开";
        ja["General.Open"] = "開く";

        en["General.Close"] = "Close";
        zh["General.Close"] = "关闭";
        ja["General.Close"] = "閉じる";

        en["General.Cancel"] = "Cancel";
        zh["General.Cancel"] = "取消";
        ja["General.Cancel"] = "キャンセル";

        en["General.OK"] = "OK";
        zh["General.OK"] = "确定";
        ja["General.OK"] = "OK";

        en["General.Apply"] = "Apply";
        zh["General.Apply"] = "应用";
        ja["General.Apply"] = "適用";

        en["General.Reset"] = "Reset";
        zh["General.Reset"] = "重置";
        ja["General.Reset"] = "リセット";

        en["General.Delete"] = "Delete";
        zh["General.Delete"] = "删除";
        ja["General.Delete"] = "削除";

        en["General.Name"] = "Name";
        zh["General.Name"] = "名称";
        ja["General.Name"] = "名前";

        en["General.Type"] = "Type";
        zh["General.Type"] = "类型";
        ja["General.Type"] = "種類";

        en["General.File"] = "File";
        zh["General.File"] = "文件";
        ja["General.File"] = "ファイル";

        en["General.Path"] = "Path";
        zh["General.Path"] = "路径";
        ja["General.Path"] = "パス";

        en["General.Error"] = "Error";
        zh["General.Error"] = "错误";
        ja["General.Error"] = "エラー";

        en["General.Success"] = "Success";
        zh["General.Success"] = "成功";
        ja["General.Success"] = "成功";

        m_translations[Language::English] = en;
        m_translations[Language::Chinese] = zh;
        m_translations[Language::Japanese] = ja;
    }

}