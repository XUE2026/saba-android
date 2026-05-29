# Saba MMD 查看器

[![Build Status](https://travis-ci.org/benikabocha/saba.svg?branch=master)](https://travis-ci.org/benikabocha/saba)
[![Build status](https://ci.appveyor.com/api/projects/status/kjk8chdx0du65m3n?svg=true)](https://ci.appveyor.com/project/benikabocha/saba)
[![Build status](https://github.com/benikabocha/saba/actions/workflows/build-linux.yml/badge.svg)](https://github.com/benikabocha/saba/actions/workflows/build-linux.yml)

[English](./README.md) | [日本語 (Japanese)](./README.jp.md)

Saba 是一个 MMD (MikuMikuDance) 模型查看与播放库，内置查看器应用程序。

## 功能特性

- 加载和显示 MMD 模型 (PMD / PMX)
- 播放 VMD 动画文件（动作、相机、灯光）
- 加载 VPD 姿势文件
- 支持 OBJ 和 X 文件格式
- 内置基于 ImGui 界面的查看器应用
- 骨骼操控与动画录制
- 环境与灯光控制
- 图像滤镜与后期处理效果
- 支持 Android 平台 (OpenGL ES 3.0)
- Lua 脚本支持，可自定义命令和自动化
- 跨平台：Windows、Linux、macOS

## 支持的文件类型

- OBJ
- PMD
- PMX
- VMD
- VPD
- X 文件 (MMD 扩展)

## 构建方法

请先安装 CMake。

### 桌面端构建

#### 所需库

- OpenGL
- [Bullet Physics](http://bulletphysics.org/wordpress/)
- [GLFW](http://www.glfw.org/)

#### 构建步骤

```bash
git clone https://github.com/XUE2026/saba.git
cd saba
mkdir build
cd build
cmake ..
make -j4
./saba_viewer
```

Release 构建：

```bash
cmake -DCMAKE_BUILD_TYPE=RELEASE ..
make -j4
```

### Android 构建

Android 项目位于 `app/` 目录下。使用 Android Studio 打开项目，通过 Gradle 构建。

前置要求：
- Android Studio Hedgehog (2023.1.1) 或更高版本
- NDK r25 或更高版本
- CMake 3.22+

```bash
./gradlew assembleDebug
```

APK 文件将生成在 `app/build/outputs/apk/debug/` 目录中。

## 初始配置

将 `init.json` 或 `init.lua` 文件放在当前目录中进行初始化配置。

详细文档请参考现有 README 中的说明。

## 许可证

本项目基于 MIT 许可证开源。

- 原作者：[benikabocha](https://github.com/benikabocha)
- 新维护者：[xueyixuan2026](https://github.com/XUE2026)

版权所有 (c) 2016-2017 benikabocha
版权所有 (c) 2026 xueyixuan2026

GitHub: https://github.com/XUE2026