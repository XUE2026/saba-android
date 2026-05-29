# Saba MMD Viewer

[![Build Status](https://travis-ci.org/benikabocha/saba.svg?branch=master)](https://travis-ci.org/benikabocha/saba)
[![Build status](https://ci.appveyor.com/api/projects/status/kjk8chdx0du65m3n?svg=true)](https://ci.appveyor.com/project/benikabocha/saba)
[![Build status](https://github.com/benikabocha/saba/actions/workflows/build-linux.yml/badge.svg)](https://github.com/benikabocha/saba/actions/workflows/build-linux.yml)

[中文 (Chinese)](./README.zh.md) | [日本語 (Japanese)](./README.jp.md)

Saba is a MMD (MikuMikuDance) model viewing and playback library with a built-in viewer application.

## Features

- Load and display MMD models (PMD / PMX)
- Play VMD animation files (motion, camera, light)
- Load VPD pose files
- Support for OBJ and X file formats
- Built-in viewer application with ImGui-based UI
- Bone manipulation and animation recording
- Environment and lighting controls
- Image filters and post-processing effects
- Android support with OpenGL ES 3.0
- Lua scripting for custom commands and automation
- Cross-platform: Windows, Linux, macOS

## File Types

- OBJ
- PMD
- PMX
- VMD
- VPD
- X file (MMD extension)

## How to Build

Please install CMake before the build.

### Desktop Build

#### Required Libraries

- OpenGL
- [Bullet Physics](http://bulletphysics.org/wordpress/)
- [GLFW](http://www.glfw.org/)

#### Build Steps

```bash
git clone https://github.com/XUE2026/saba.git
cd saba
mkdir build
cd build
cmake ..
make -j4
./saba_viewer
```

For release build:

```bash
cmake -DCMAKE_BUILD_TYPE=RELEASE ..
make -j4
```

### Android Build

The Android project is located in the `app/` directory. Open the project in Android Studio and build it using Gradle.

Prerequisites:
- Android Studio Hedgehog (2023.1.1) or later
- NDK r25 or later
- CMake 3.22+

```bash
./gradlew assembleDebug
```

The APK will be generated at `app/build/outputs/apk/debug/`.

## Initial Setting

Initialize with the `init.json` or `init.lua` file placed in the current directory.

See the detailed documentation in the existing README for more information on configuration.

## License

This project is licensed under the MIT License.

- Original author: [benikabocha](https://github.com/benikabocha)
- New maintainer: [xueyixuan2026](https://github.com/XUE2026)

Copyright (c) 2016-2017 benikabocha
Copyright (c) 2026 xueyixuan2026

GitHub: https://github.com/XUE2026