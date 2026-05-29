# Saba MMD Viewer for Android

A powerful MMD (MikuMikuDance) model viewer for Android, ported from [benikabocha/saba](https://github.com/benikabocha/saba).

## Features

- **Model Support**: PMX, PMD, FBX, OBJ, X File formats
- **Motion Support**: VMD animation playback, action recording
- **3D Controls**: Full camera orbit/dolly/pan via touch gestures
- **Filters**: White Bright, Skin Bright, Dark, Genshin Impact (Day/Night/Dusk)
- **Environment**: Fully adjustable sky, ground, lighting, toon intensity
- **Bone Control**: Manual bone manipulation, built-in actions (Jump/Run/Walk)
- **Action Recording**: Record and export motions, split into segments with gap adjustment
- **Texture Replacement**: PMX texture replacement and clothing addition
- **Scene Import**: Scene file import and environment customization
- **Built-in Primitives**: Sun, Cube, Sphere, Cone, Cylinder
- **Built-in Environments**: Floor, Sky, Field, Dirt, Asphalt
- **Multi-language**: English, 中文, 日本語
- **Orientation**: Portrait/Landscape/Auto
- **Gobot Framework**: .gobot action/scene file support (on gobot branch)

## Screenshots

![Saba Viewer](images/saba_viewer_01.png)

## Build Instructions

### Prerequisites

- Android Studio Hedgehog (2023.1.1) or later
- JDK 17
- Android SDK 34
- Android NDK 25.2.9519653
- CMake 3.22.1+

### Quick Start

```bash
# Clone the repository
git clone https://github.com/XUE2026/Saba-Android.git
cd Saba-Android

# Switch to development branch
git checkout environment

# Build with Gradle
cd android
./gradlew assembleDebug
```

### GitHub Actions

The project includes GitHub Actions workflow (`.github/workflows/build-android.yml`) that automatically builds the APK on push to `environment`, `gobot`, or `master` branches.

## Project Structure

```
Saba-Android/
├── android/                    # Android project
│   ├── app/
│   │   ├── src/
│   │   │   ├── main/
│   │   │   │   ├── java/      # Java source (Activity, GLSurfaceView)
│   │   │   │   ├── cpp/       # C++ native code
│   │   │   │   ├── res/       # Resources (layouts, strings, themes)
│   │   │   │   └── AndroidManifest.xml
│   │   │   └── test/          # Unit tests
│   │   ├── build.gradle
│   │   └── proguard-rules.pro
│   ├── gradle/wrapper/
│   ├── build.gradle
│   └── settings.gradle
├── src/                       # Saba library source (C++)
├── viewer/                    # Saba viewer source (C++)
├── external/                  # Third-party dependencies
├── .github/workflows/         # CI/CD configurations
└── docs/                      # Documentation
```

## Architecture

```
┌─────────────────────────────────────┐
│         Android Activity Layer       │
│  (MainActivity, NativeGLSurfaceView) │
├─────────────────────────────────────┤
│          JNI Bridge (jni_bridge.cpp) │
├─────────────────────────────────────┤
│      Native Engine (C++)             │
│  ┌───────────────────────────────┐   │
│  │ AndroidViewer (Main Controller)│   │
│  ├───────────────────────────────┤   │
│  │ FilterSystem  │ Environment   │   │
│  │ ActionRecorder│ BoneController│   │
│  ├───────────────────────────────┤   │
│  │ Saba Engine (MMD/PMX/VMD)    │   │
│  │ OpenGL ES 3.0 Renderer       │   │
│  └───────────────────────────────┘   │
├─────────────────────────────────────┤
│         EGL + OpenGL ES 3.0         │
└─────────────────────────────────────┘
```

## Development

### Branches

- `master` - Stable releases
- `environment` - Main development branch
- `gobot` - Gobot framework integration branch

### Adding New Features

1. Branch from `environment`
2. Make changes
3. Ensure APK builds via GitHub Actions
4. Submit pull request

## License

Original Copyright: benikabocha (MIT License)
New Copyright: xueyixuan2026

This project is licensed under the MIT License - see the [LICENSE](LICENCE) file for details.

## Author

- **xueyixuan2026** - [GitHub](https://github.com/XUE2026)

## Acknowledgments

- [benikabocha/saba](https://github.com/benikabocha/saba) - Original MMD viewer library
- MikuMikuDance (MMD) - Vocaloid promotion video project