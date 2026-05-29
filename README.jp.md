# Saba MMD ビューア

[![Build Status](https://travis-ci.org/benikabocha/saba.svg?branch=master)](https://travis-ci.org/benikabocha/saba)
[![Build status](https://ci.appveyor.com/api/projects/status/kjk8chdx0du65m3n?svg=true)](https://ci.appveyor.com/project/benikabocha/saba)
[![Build status](https://github.com/benikabocha/saba/actions/workflows/build-linux.yml/badge.svg)](https://github.com/benikabocha/saba/actions/workflows/build-linux.yml)

[English](./README.md) | [中文 (Chinese)](./README.zh.md)

Saba は MMD (MikuMikuDance) モデルの表示・再生ライブラリで、ビューアアプリケーションを内蔵しています。

## 特徴

- MMD モデル (PMD / PMX) の読み込みと表示
- VMD アニメーションファイルの再生（モーション、カメラ、照明）
- VPD ポーズファイルの読み込み
- OBJ および X ファイル形式のサポート
- ImGui ベースのビューアアプリケーション
- ボーン操作とアニメーション録画
- 環境と照明の制御
- 画像フィルターとポストプロセスエフェクト
- Android 対応 (OpenGL ES 3.0)
- Lua スクリプトによるカスタムコマンドと自動化
- クロスプラットフォーム：Windows、Linux、macOS

## 対応ファイル形式

- OBJ
- PMD
- PMX
- VMD
- VPD
- X ファイル (MMD 拡張)

## ビルド方法

ビルドには CMake が必要です。事前にインストールしてください。

### デスクトップビルド

#### 必要なライブラリ

- OpenGL
- [Bullet Physics](http://bulletphysics.org/wordpress/)
- [GLFW](http://www.glfw.org/)

#### ビルド手順

```bash
git clone https://github.com/XUE2026/saba.git
cd saba
mkdir build
cd build
cmake ..
make -j4
./saba_viewer
```

リリースビルド：

```bash
cmake -DCMAKE_BUILD_TYPE=RELEASE ..
make -j4
```

### Android ビルド

Android プロジェクトは `app/` ディレクトリにあります。Android Studio でプロジェクトを開き、Gradle でビルドしてください。

前提条件：
- Android Studio Hedgehog (2023.1.1) 以降
- NDK r25 以降
- CMake 3.22+

```bash
./gradlew assembleDebug
```

APK は `app/build/outputs/apk/debug/` に生成されます。

## 初期化設定

起動時のカレントディレクトリに `init.json` または `init.lua` ファイルを配置して初期化設定を行います。

詳細な設定については既存の README を参照してください。

## ライセンス

このプロジェクトは MIT ライセンスの下で公開されています。

- 原作者：[benikabocha](https://github.com/benikabocha)
- 新メンテナー：[xueyixuan2026](https://github.com/XUE2026)

Copyright (c) 2016-2017 benikabocha
Copyright (c) 2026 xueyixuan2026

GitHub: https://github.com/XUE2026