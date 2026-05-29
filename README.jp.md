# Saba MMD ビューアー Android版

Android 向けの強力な MMD (MikuMikuDance) モデルビューアー。[benikabocha/saba](https://github.com/benikabocha/saba) から移植。

## 機能

- **モデル対応**: PMX, PMD, FBX, OBJ, X File 形式
- **モーション対応**: VMD アニメーション再生、アクション録画
- **3D コントロール**: タッチジェスチャーによるカメラ操作（オービット/ドリー/パン）
- **フィルター**: 白明るい、肌色、暗い、原神フィルター（昼/夜/夕暮れ）
- **環境**: 空、地面、照明、トーン強度を完全調整可能
- **ボーン制御**: 手動ボーン操作、内蔵アクション（ジャンプ/走る/歩く）
- **アクション録画**: モーションの録画とエクスポート、セグメント分割とギャップ調整
- **テクスチャ置換**: PMX テクスチャの置換と衣装追加
- **シーンインポート**: シーンファイルのインポートと環境カスタマイズ
- **内蔵プリミティブ**: 太陽、立方体、球体、円錐、円柱
- **内蔵環境**: 床、空、野原、土、アスファルト
- **多言語**: English, 中文, 日本語
- **画面方向**: 縦向き/横向き/自動
- **Gobot フレームワーク**: .gobot アクション/シーンファイル対応（gobot ブランチ）

## ビルド手順

### 前提条件

- Android Studio Hedgehog (2023.1.1) 以降
- JDK 17
- Android SDK 34
- Android NDK 25.2.9519653
- CMake 3.22.1+

### クイックスタート

```bash
git clone https://github.com/XUE2026/Saba-Android.git
cd Saba-Android
git checkout environment
cd android
./gradlew assembleDebug
```

### GitHub Actions

`.github/workflows/build-android.yml` により、`environment`、`gobot`、`master` ブランチへのプッシュ時に自動的に APK がビルドされます。

## プロジェクト構造

```
Saba-Android/
├── android/                    # Android プロジェクト
│   ├── app/
│   │   ├── src/
│   │   │   ├── main/
│   │   │   │   ├── java/      # Java ソース
│   │   │   │   ├── cpp/       # C++ ネイティブコード
│   │   │   │   ├── res/       # リソース
│   │   │   │   └── AndroidManifest.xml
│   │   │   └── test/
│   │   ├── build.gradle
│   │   └── proguard-rules.pro
│   ├── gradle/wrapper/
│   ├── build.gradle
│   └── settings.gradle
├── src/                       # Saba ライブラリソース (C++)
├── viewer/                    # Saba ビューアーソース (C++)
├── external/                  # サードパーティ依存関係
├── .github/workflows/         # CI/CD 設定
```

## アーキテクチャ

```
┌─────────────────────────────────────┐
│         Android Activity 層          │
│  (MainActivity, NativeGLSurfaceView) │
├─────────────────────────────────────┤
│          JNI ブリッジ (jni_bridge.cpp)│
├─────────────────────────────────────┤
│      ネイティブエンジン (C++)          │
│  ┌───────────────────────────────┐   │
│  │ AndroidViewer (メインコントローラ)│   │
│  ├───────────────────────────────┤   │
│  │ FilterSystem │ Environment   │   │
│  │ ActionRecorder│ BoneController│   │
│  ├───────────────────────────────┤   │
│  │ Saba エンジン (MMD/PMX/VMD)   │   │
│  │ OpenGL ES 3.0 レンダラー      │   │
│  └───────────────────────────────┘   │
├─────────────────────────────────────┤
│         EGL + OpenGL ES 3.0         │
└─────────────────────────────────────┘
```

## 開発

### ブランチ

- `master` - 安定版
- `environment` - メイン開発ブランチ
- `gobot` - Gobot フレームワーク統合ブランチ

## ライセンス

オリジナル著作権: benikabocha (MIT License)
新著作権: xueyixuan2026

このプロジェクトは MIT ライセンスの下で提供されています。[LICENSE](LICENCE) ファイルをご覧ください。

## 作者

- **xueyixuan2026** - [GitHub](https://github.com/XUE2026)

## 謝辞

- [benikabocha/saba](https://github.com/benikabocha/saba) - オリジナル MMD ビューアーライブラリ
- MikuMikuDance (MMD) - ボーカロイドプロモーションビデオプロジェクト