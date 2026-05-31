//
// Copyright(c) 2016-2017 benikabocha.
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
		: m_currentLang(English)
	{
		LoadStrings();
	}

	LocalizationManager::~LocalizationManager()
	{
	}

	void LocalizationManager::SetLanguage(Language lang)
	{
		m_currentLang = lang;
	}

	LocalizationManager::Language LocalizationManager::GetLanguage() const
	{
		return m_currentLang;
	}

	std::string LocalizationManager::GetString(const std::string& key) const
	{
		const StringMap* map = nullptr;
		switch (m_currentLang)
		{
		case Chinese:
			map = &m_zhStrings;
			break;
		case Japanese:
			map = &m_jaStrings;
			break;
		default:
			map = &m_enStrings;
			break;
		}

		auto it = map->find(key);
		if (it != map->end())
		{
			return it->second;
		}

		auto enIt = m_enStrings.find(key);
		if (enIt != m_enStrings.end())
		{
			return enIt->second;
		}

		return key;
	}

	void LocalizationManager::LoadStrings()
	{
		m_enStrings["loadModel"] = "Load Model";
		m_enStrings["unloadModel"] = "Unload Model";
		m_enStrings["modelInfo"] = "Model Info";
		m_enStrings["vertices"] = "Vertices";
		m_enStrings["faces"] = "Faces";
		m_enStrings["bones"] = "Bones";
		m_enStrings["materials"] = "Materials";
		m_enStrings["textures"] = "Textures";
		m_enStrings["replaceTexture"] = "Replace Texture";
		m_enStrings["addClothing"] = "Add Clothing";
		m_enStrings["filter"] = "Filter";
		m_enStrings["preset"] = "Preset";
		m_enStrings["whiteBright"] = "White Bright";
		m_enStrings["skinBright"] = "Skin Bright";
		m_enStrings["dark"] = "Dark";
		m_enStrings["genshinDay"] = "Genshin Day";
		m_enStrings["genshinNight"] = "Genshin Night";
		m_enStrings["genshinDusk"] = "Genshin Dusk";
		m_enStrings["exposure"] = "Exposure";
		m_enStrings["contrast"] = "Contrast";
		m_enStrings["saturation"] = "Saturation";
		m_enStrings["hueShift"] = "Hue Shift";
		m_enStrings["temperature"] = "Temperature";
		m_enStrings["tint"] = "Tint";
		m_enStrings["resetFilter"] = "Reset Filter";
		m_enStrings["environment"] = "Environment";
		m_enStrings["sunDirection"] = "Sun Direction";
		m_enStrings["sunIntensity"] = "Sun Intensity";
		m_enStrings["sunColor"] = "Sun Color";
		m_enStrings["ground"] = "Ground";
		m_enStrings["grid"] = "Grid";
		m_enStrings["checker"] = "Checker";
		m_enStrings["grass"] = "Grass";
		m_enStrings["dirt"] = "Dirt";
		m_enStrings["asphalt"] = "Asphalt";
		m_enStrings["addPrimitive"] = "Add Primitive";
		m_enStrings["cube"] = "Cube";
		m_enStrings["sphere"] = "Sphere";
		m_enStrings["cone"] = "Cone";
		m_enStrings["cylinder"] = "Cylinder";
		m_enStrings["boneList"] = "Bone List";
		m_enStrings["rotate"] = "Rotate";
		m_enStrings["translate"] = "Translate";
		m_enStrings["reset"] = "Reset";
		m_enStrings["selectBone"] = "Select Bone";
		m_enStrings["animation"] = "Animation";
		m_enStrings["idle"] = "Idle";
		m_enStrings["walk"] = "Walk";
		m_enStrings["run"] = "Run";
		m_enStrings["jump"] = "Jump";
		m_enStrings["stop"] = "Stop";
		m_enStrings["play"] = "Play";
		m_enStrings["speed"] = "Speed";
		m_enStrings["recording"] = "Recording";
		m_enStrings["startRecord"] = "Start Record";
		m_enStrings["stopRecord"] = "Stop Record";
		m_enStrings["export"] = "Export";
		m_enStrings["split"] = "Split";
		m_enStrings["gapTime"] = "Gap Time";
		m_enStrings["playSegment"] = "Play Segment";
		m_enStrings["settings"] = "Settings";
		m_enStrings["language"] = "Language";
		m_enStrings["screenOrientation"] = "Screen Orientation";
		m_enStrings["portrait"] = "Portrait";
		m_enStrings["landscape"] = "Landscape";
		m_enStrings["auto"] = "Auto";
		m_enStrings["cacheSize"] = "Cache Size";
		m_enStrings["clearCache"] = "Clear Cache";
		m_enStrings["loading"] = "Loading...";
		m_enStrings["success"] = "Success";
		m_enStrings["error"] = "Error";
		m_enStrings["confirm"] = "Confirm";
		m_enStrings["cancel"] = "Cancel";
		m_enStrings["close"] = "Close";

		m_zhStrings["loadModel"] = "加载模型";
		m_zhStrings["unloadModel"] = "卸载模型";
		m_zhStrings["modelInfo"] = "模型信息";
		m_zhStrings["vertices"] = "顶点数";
		m_zhStrings["faces"] = "面数";
		m_zhStrings["bones"] = "骨骼";
		m_zhStrings["materials"] = "材质";
		m_zhStrings["textures"] = "纹理";
		m_zhStrings["replaceTexture"] = "替换纹理";
		m_zhStrings["addClothing"] = "添加服装";
		m_zhStrings["filter"] = "滤镜";
		m_zhStrings["preset"] = "预设";
		m_zhStrings["whiteBright"] = "明亮白";
		m_zhStrings["skinBright"] = "亮肤";
		m_zhStrings["dark"] = "暗黑";
		m_zhStrings["genshinDay"] = "原神·昼";
		m_zhStrings["genshinNight"] = "原神·夜";
		m_zhStrings["genshinDusk"] = "原神·黄昏";
		m_zhStrings["exposure"] = "曝光";
		m_zhStrings["contrast"] = "对比度";
		m_zhStrings["saturation"] = "饱和度";
		m_zhStrings["hueShift"] = "色相偏移";
		m_zhStrings["temperature"] = "色温";
		m_zhStrings["tint"] = "色调";
		m_zhStrings["resetFilter"] = "重置滤镜";
		m_zhStrings["environment"] = "环境";
		m_zhStrings["sunDirection"] = "太阳方向";
		m_zhStrings["sunIntensity"] = "太阳强度";
		m_zhStrings["sunColor"] = "太阳颜色";
		m_zhStrings["ground"] = "地面";
		m_zhStrings["grid"] = "网格";
		m_zhStrings["checker"] = "方格";
		m_zhStrings["grass"] = "草地";
		m_zhStrings["dirt"] = "泥土";
		m_zhStrings["asphalt"] = "柏油路";
		m_zhStrings["addPrimitive"] = "添加基元";
		m_zhStrings["cube"] = "立方体";
		m_zhStrings["sphere"] = "球体";
		m_zhStrings["cone"] = "圆锥";
		m_zhStrings["cylinder"] = "圆柱";
		m_zhStrings["boneList"] = "骨骼列表";
		m_zhStrings["rotate"] = "旋转";
		m_zhStrings["translate"] = "平移";
		m_zhStrings["reset"] = "重置";
		m_zhStrings["selectBone"] = "选择骨骼";
		m_zhStrings["animation"] = "动画";
		m_zhStrings["idle"] = "待机";
		m_zhStrings["walk"] = "行走";
		m_zhStrings["run"] = "奔跑";
		m_zhStrings["jump"] = "跳跃";
		m_zhStrings["stop"] = "停止";
		m_zhStrings["play"] = "播放";
		m_zhStrings["speed"] = "速度";
		m_zhStrings["recording"] = "录制";
		m_zhStrings["startRecord"] = "开始录制";
		m_zhStrings["stopRecord"] = "停止录制";
		m_zhStrings["export"] = "导出";
		m_zhStrings["split"] = "分割";
		m_zhStrings["gapTime"] = "间隔时间";
		m_zhStrings["playSegment"] = "播放片段";
		m_zhStrings["settings"] = "设置";
		m_zhStrings["language"] = "语言";
		m_zhStrings["screenOrientation"] = "屏幕方向";
		m_zhStrings["portrait"] = "竖屏";
		m_zhStrings["landscape"] = "横屏";
		m_zhStrings["auto"] = "自动";
		m_zhStrings["cacheSize"] = "缓存大小";
		m_zhStrings["clearCache"] = "清除缓存";
		m_zhStrings["loading"] = "加载中...";
		m_zhStrings["success"] = "成功";
		m_zhStrings["error"] = "错误";
		m_zhStrings["confirm"] = "确认";
		m_zhStrings["cancel"] = "取消";
		m_zhStrings["close"] = "关闭";

		m_jaStrings["loadModel"] = "モデルを読み込む";
		m_jaStrings["unloadModel"] = "モデルを解放";
		m_jaStrings["modelInfo"] = "モデル情報";
		m_jaStrings["vertices"] = "頂点数";
		m_jaStrings["faces"] = "面数";
		m_jaStrings["bones"] = "ボーン";
		m_jaStrings["materials"] = "マテリアル";
		m_jaStrings["textures"] = "テクスチャ";
		m_jaStrings["replaceTexture"] = "テクスチャ置換";
		m_jaStrings["addClothing"] = "衣装追加";
		m_jaStrings["filter"] = "フィルター";
		m_jaStrings["preset"] = "プリセット";
		m_jaStrings["whiteBright"] = "ホワイトブライト";
		m_jaStrings["skinBright"] = "スキンブライト";
		m_jaStrings["dark"] = "ダーク";
		m_jaStrings["genshinDay"] = "原神・昼";
		m_jaStrings["genshinNight"] = "原神・夜";
		m_jaStrings["genshinDusk"] = "原神・夕暮れ";
		m_jaStrings["exposure"] = "露出";
		m_jaStrings["contrast"] = "コントラスト";
		m_jaStrings["saturation"] = "彩度";
		m_jaStrings["hueShift"] = "色相シフト";
		m_jaStrings["temperature"] = "色温度";
		m_jaStrings["tint"] = "ティント";
		m_jaStrings["resetFilter"] = "フィルターリセット";
		m_jaStrings["environment"] = "環境";
		m_jaStrings["sunDirection"] = "太陽の方向";
		m_jaStrings["sunIntensity"] = "太陽の強さ";
		m_jaStrings["sunColor"] = "太陽の色";
		m_jaStrings["ground"] = "地面";
		m_jaStrings["grid"] = "グリッド";
		m_jaStrings["checker"] = "チェッカー";
		m_jaStrings["grass"] = "芝生";
		m_jaStrings["dirt"] = "土";
		m_jaStrings["asphalt"] = "アスファルト";
		m_jaStrings["addPrimitive"] = "プリミティブ追加";
		m_jaStrings["cube"] = "立方体";
		m_jaStrings["sphere"] = "球体";
		m_jaStrings["cone"] = "円錐";
		m_jaStrings["cylinder"] = "円柱";
		m_jaStrings["boneList"] = "ボーン一覧";
		m_jaStrings["rotate"] = "回転";
		m_jaStrings["translate"] = "移動";
		m_jaStrings["reset"] = "リセット";
		m_jaStrings["selectBone"] = "ボーン選択";
		m_jaStrings["animation"] = "アニメーション";
		m_jaStrings["idle"] = "待機";
		m_jaStrings["walk"] = "歩く";
		m_jaStrings["run"] = "走る";
		m_jaStrings["jump"] = "ジャンプ";
		m_jaStrings["stop"] = "停止";
		m_jaStrings["play"] = "再生";
		m_jaStrings["speed"] = "速度";
		m_jaStrings["recording"] = "録画";
		m_jaStrings["startRecord"] = "録画開始";
		m_jaStrings["stopRecord"] = "録画停止";
		m_jaStrings["export"] = "エクスポート";
		m_jaStrings["split"] = "分割";
		m_jaStrings["gapTime"] = "ギャップ時間";
		m_jaStrings["playSegment"] = "セグメント再生";
		m_jaStrings["settings"] = "設定";
		m_jaStrings["language"] = "言語";
		m_jaStrings["screenOrientation"] = "画面の向き";
		m_jaStrings["portrait"] = "縦向き";
		m_jaStrings["landscape"] = "横向き";
		m_jaStrings["auto"] = "自動";
		m_jaStrings["cacheSize"] = "キャッシュサイズ";
		m_jaStrings["clearCache"] = "キャッシュクリア";
		m_jaStrings["loading"] = "読み込み中...";
		m_jaStrings["success"] = "成功";
		m_jaStrings["error"] = "エラー";
		m_jaStrings["confirm"] = "確認";
		m_jaStrings["cancel"] = "キャンセル";
		m_jaStrings["close"] = "閉じる";
	}
}