#if UNITY_EDITOR
using System.IO;
using UnityEditor;
using UnityEditor.Build;
using UnityEditor.Build.Reporting;
using UnityEngine;

namespace Saba.MMDViewer.Editor
{
    public class BuildHelper
    {
        private const string BuildOutputDir = "Builds/Android";

        [MenuItem("Saba/Build/Android APK", false, 10)]
        public static void BuildAndroidAPK()
        {
            string[] scenes = GetEnabledScenes();
            if (scenes.Length == 0)
            {
                Debug.LogError("[BuildHelper] No enabled scenes found in build settings.");
                return;
            }

            if (!Directory.Exists(BuildOutputDir))
            {
                Directory.CreateDirectory(BuildOutputDir);
            }

            string apkPath = Path.Combine(BuildOutputDir, "SabaMMDViewer.apk");

            PlayerSettings.SetScriptingBackend(BuildTargetGroup.Android, ScriptingImplementation.IL2CPP);
            PlayerSettings.SetApiCompatibilityLevel(BuildTargetGroup.Android, ApiCompatibilityLevel.NET_Unity_4_8);
            PlayerSettings.Android.targetArchitectures = AndroidArchitecture.ARM64 | AndroidArchitecture.ARMv7;
            PlayerSettings.Android.minSdkVersion = AndroidSdkVersions.AndroidApiLevel29;
            PlayerSettings.Android.targetSdkVersion = AndroidSdkVersions.AndroidApiLevel33;
            PlayerSettings.Android.preferredInstallLocation = AndroidPreferredInstallLocation.Auto;

            PlayerSettings.defaultScreenOrientation = ScreenOrientation.AutoRotation;
            PlayerSettings.allowedAutorotateToPortrait = true;
            PlayerSettings.allowedAutorotateToPortraitUpsideDown = false;
            PlayerSettings.allowedAutorotateToLandscapeLeft = true;
            PlayerSettings.allowedAutorotateToLandscapeRight = true;

            PlayerSettings.graphicsAPIsForAndroid = new UnityEngine.Rendering.GraphicsDeviceType[]
            {
                UnityEngine.Rendering.GraphicsDeviceType.Vulkan,
                UnityEngine.Rendering.GraphicsDeviceType.OpenGLES3
            };

            PlayerSettings.stripEngineCode = true;
            PlayerSettings.SetStackTraceLogType(LogType.Log, StackTraceLogType.None);
            PlayerSettings.SetStackTraceLogType(LogType.Warning, StackTraceLogType.None);
            PlayerSettings.SetStackTraceLogType(LogType.Error, StackTraceLogType.ScriptOnly);
            PlayerSettings.SetStackTraceLogType(LogType.Assert, StackTraceLogType.ScriptOnly);
            PlayerSettings.SetStackTraceLogType(LogType.Exception, StackTraceLogType.ScriptOnly);

            QualitySettings.SetQualityLevel(2, true);
            QualitySettings.antiAliasing = 4;
            QualitySettings.anisotropicFiltering = AnisotropicFiltering.Enable;
            QualitySettings.vSyncCount = 0;
            QualitySettings.shadows = ShadowQuality.All;
            QualitySettings.shadowResolution = ShadowResolution.Medium;

            BuildPlayerOptions buildOptions = new BuildPlayerOptions
            {
                scenes = scenes,
                locationPathName = apkPath,
                targetGroup = BuildTargetGroup.Android,
                target = BuildTarget.Android,
                options = BuildOptions.CompressWithLz4HC | BuildOptions.StrictMode
            };

            BuildReport report = BuildPipeline.BuildPlayer(buildOptions);
            BuildSummary summary = report.summary;

            if (summary.result == BuildResult.Succeeded)
            {
                Debug.Log($"[BuildHelper] Build succeeded: {apkPath} ({summary.totalSize / 1048576.0:F2} MB)");
            }
            else
            {
                Debug.LogError($"[BuildHelper] Build failed: {summary.result}");
                foreach (var step in report.steps)
                {
                    foreach (var message in step.messages)
                    {
                        if (message.type == LogType.Error || message.type == LogType.Exception)
                        {
                            Debug.LogError($"[BuildHelper] {message.content}");
                        }
                    }
                }
            }
        }

        [MenuItem("Saba/Build/Android APK (Development)", false, 11)]
        public static void BuildAndroidAPKDevelopment()
        {
            string[] scenes = GetEnabledScenes();
            if (scenes.Length == 0)
            {
                Debug.LogError("[BuildHelper] No enabled scenes found in build settings.");
                return;
            }

            if (!Directory.Exists(BuildOutputDir))
            {
                Directory.CreateDirectory(BuildOutputDir);
            }

            string apkPath = Path.Combine(BuildOutputDir, "SabaMMDViewer_Dev.apk");

            BuildPlayerOptions buildOptions = new BuildPlayerOptions
            {
                scenes = scenes,
                locationPathName = apkPath,
                targetGroup = BuildTargetGroup.Android,
                target = BuildTarget.Android,
                options = BuildOptions.Development | BuildOptions.AllowDebugging |
                         BuildOptions.CompressWithLz4HC | BuildOptions.StrictMode
            };

            BuildReport report = BuildPipeline.BuildPlayer(buildOptions);
            BuildSummary summary = report.summary;

            if (summary.result == BuildResult.Succeeded)
            {
                Debug.Log($"[BuildHelper] Development build succeeded: {apkPath} ({summary.totalSize / 1048576.0:F2} MB)");
            }
            else
            {
                Debug.LogError($"[BuildHelper] Development build failed: {summary.result}");
            }
        }

        [MenuItem("Saba/Build/Configure Android Settings", false, 20)]
        public static void ConfigureAndroidSettings()
        {
            PlayerSettings.productName = "Saba MMD Viewer";
            PlayerSettings.companyName = "Saba";
            PlayerSettings.bundleVersion = "1.0.0";
            PlayerSettings.applicationIdentifier = "com.saba.mmdviewer";

            PlayerSettings.Android.forceInternetPermission = false;
            PlayerSettings.Android.forceSDCardPermission = false;
            PlayerSettings.Android.useAPKExpansionFiles = false;
            PlayerSettings.Android.showActivityIndicatorOnLoading = AndroidShowActivityIndicatorOnLoading.DontShow;

            PlayerSettings.Android.androidTVCompatibility = false;
            PlayerSettings.Android.androidGamepadSupportLevel = AndroidGamepadSupportLevel.Requires;

            PlayerSettings.SetUseDefaultGraphicsAPIsForAndroid(BuildTarget.Android, false);

            Debug.Log("[BuildHelper] Android settings configured.");
        }

        [MenuItem("Saba/Build/Clean Build Output", false, 30)]
        public static void CleanBuildOutput()
        {
            if (Directory.Exists(BuildOutputDir))
            {
                Directory.Delete(BuildOutputDir, true);
                Debug.Log($"[BuildHelper] Cleaned build output: {BuildOutputDir}");
            }
            else
            {
                Debug.Log("[BuildHelper] No build output to clean.");
            }
        }

        private static string[] GetEnabledScenes()
        {
            EditorBuildSettingsScene[] scenes = EditorBuildSettings.scenes;
            if (scenes == null || scenes.Length == 0)
            {
                string[] defaultScenes = { "Assets/Scenes/MainScene.unity" };
                return defaultScenes;
            }

            string[] enabledScenes = new string[scenes.Length];
            int count = 0;
            foreach (EditorBuildSettingsScene scene in scenes)
            {
                if (scene.enabled)
                {
                    enabledScenes[count++] = scene.path;
                }
            }

            if (count == 0)
            {
                return new string[] { "Assets/Scenes/MainScene.unity" };
            }

            System.Array.Resize(ref enabledScenes, count);
            return enabledScenes;
        }
    }

    public class BuildPreprocessor : IPreprocessBuildWithReport
    {
        public int callbackOrder => 0;

        public void OnPreprocessBuild(BuildReport report)
        {
            if (report.summary.platform == BuildTarget.Android)
            {
                Debug.Log($"[BuildPreprocessor] Preprocessing Android build: {report.summary.outputPath}");

                if (!PlayerSettings.Android.allowBackup)
                {
                    PlayerSettings.Android.allowBackup = false;
                }
            }
        }
    }

    public class BuildPostprocessor : IPostprocessBuildWithReport
    {
        public int callbackOrder => 0;

        public void OnPostprocessBuild(BuildReport report)
        {
            if (report.summary.platform == BuildTarget.Android)
            {
                Debug.Log($"[BuildPostprocessor] Android build completed: {report.summary.outputPath}");
                Debug.Log($"[BuildPostprocessor] Build size: {report.summary.totalSize / 1048576.0:F2} MB");
                Debug.Log($"[BuildPostprocessor] Build time: {report.summary.buildStartedAt:HH:mm:ss} - {report.summary.buildEndedAt:HH:mm:ss}");
            }
        }
    }
}
#endif