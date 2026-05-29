# Saba MMD 查看器 Android版

一个功能强大的 Android MMD (MikuMikuDance) 模型查看器，从 [benikabocha/saba](https://github.com/benikabocha/saba) 移植。

## 功能特点

- **模型支持**: PMX, PMD, FBX, OBJ, X File 格式
- **动作支持**: VMD 动画播放，动作录制
- **3D 控制**: 通过触摸手势实现完整的视角轨道/推拉/平移
- **滤镜**: 白亮滤镜，肤亮滤镜，黑暗滤镜，原神滤镜(白天/晚上/黄昏)
- **环境**: 完全可调的天空、地面、光照、漫画强度
- **骨骼控制**: 手动骨骼调整，内置动作(跳跃/奔跑/行走)
- **动作录制**: 录制和导出动作，拆分为片段并支持间隙调整
- **纹理替换**: PMX 纹理替换和衣物增添
- **场景导入**: 场景文件导入和环境自定义
- **内置几何体**: 太阳、正方体、球体、圆锥、圆柱
- **内置环境**: 地板、天空、田野、泥土地、柏油地面
- **多语言**: English, 中文, 日本語
- **屏幕方向**: 竖向/横向/自动
- **Gobot 框架**: .gobot 动作/场景文件支持(在 gobot 分支)

## 构建说明

### 前提条件

- Android Studio Hedgehog (2023.1.1) 或更高版本
- JDK 17
- Android SDK 34
- Android NDK 25.2.9519653
- CMake 3.22.1+

### 快速开始

```bash
git clone https://github.com/XUE2026/Saba-Android.git
cd Saba-Android
git checkout environment
cd android
./gradlew assembleDebug
```

### GitHub Actions

项目包含 GitHub Actions 工作流 (`.github/workflows/build-android.yml`)，会在推送到 `environment`、`gobot` 或 `master` 分支时自动构建 APK。

## 项目结构

```
Saba-Android/
├── android/                    # Android 项目
│   ├── app/
│   │   ├── src/
│   │   │   ├── main/
│   │   │   │   ├── java/      # Java 源码
│   │   │   │   ├── cpp/       # C++ 原生代码
│   │   │   │   ├── res/       # 资源文件
│   │   │   │   └── AndroidManifest.xml
│   │   │   └── test/
│   │   ├── build.gradle
│   │   └── proguard-rules.pro
│   ├── gradle/wrapper/
│   ├── build.gradle
│   └── settings.gradle
├── src/                       # Saba 库源码 (C++)
├── viewer/                    # Saba 查看器源码 (C++)
├── external/                  # 第三方依赖
├── .github/workflows/         # CI/CD 配置
```

## 架构

```
┌─────────────────────────────────────┐
│         Android Activity 层          │
│  (MainActivity, NativeGLSurfaceView) │
├─────────────────────────────────────┤
│          JNI 桥接 (jni_bridge.cpp)    │
├─────────────────────────────────────┤
│      原生引擎 (C++)                   │
│  ┌───────────────────────────────┐   │
│  │ AndroidViewer (主控制器)       │   │
│  ├───────────────────────────────┤   │
│  │ FilterSystem │ Environment   │   │
│  │ ActionRecorder│ BoneController│   │
│  ├───────────────────────────────┤   │
│  │ Saba 引擎 (MMD/PMX/VMD)      │   │
│  │ OpenGL ES 3.0 渲染器          │   │
│  └───────────────────────────────┘   │
├─────────────────────────────────────┤
│         EGL + OpenGL ES 3.0         │
└─────────────────────────────────────┘
```

## 开发

### 分支

- `master` - 稳定版本
- `environment` - 主开发分支
- `gobot` - Gobot 框架集成分支

## 许可证

原始版权: benikabocha (MIT License)
新版权: xueyixuan2026

本项目采用 MIT 许可证 - 查看 [LICENSE](LICENCE) 文件了解详情。

## 作者

- **xueyixuan2026** - [GitHub](https://github.com/XUE2026)

## 致谢

- [benikabocha/saba](https://github.com/benikabocha/saba) - 原始 MMD 查看器库
- MikuMikuDance (MMD) - 声音动画制作项目