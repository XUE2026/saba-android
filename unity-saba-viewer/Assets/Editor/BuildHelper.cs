using UnityEditor;
using UnityEditor.Build;
using UnityEditor.Build.Reporting;
using UnityEngine;

namespace UnitySabaViewer.Editor
{
    public class BuildHelper
    {
        private const string BuildPath = "Builds/Android";

        [MenuItem("SabaViewer/Build Android APK")]
        public static void BuildAndroid()
        {
            string[] scenes = GetEnabledScenes();

            if (scenes.Length == 0)
            {
                Debug.LogError("[BuildHelper] No enabled scenes found in build settings");
                return;
            }

            string outputPath = $"{BuildPath}/SabaViewer.apk";

            PlayerSettings.SetScriptingBackend(BuildTargetGroup.Android, ScriptingImplementation.IL2CPP);
            PlayerSettings.SetApiCompatibilityLevel(BuildTargetGroup.Android, ApiCompatibilityLevel.NET_Unity_4_8);
            PlayerSettings.Android.targetArchitectures = AndroidArchitecture.ARM64;
            PlayerSettings.Android.minSdkVersion = AndroidSdkVersions.AndroidApiLevel22;
            PlayerSettings.Android.targetSdkVersion = AndroidSdkVersions.AndroidApiLevel32;

            PlayerSettings.productName = "SabaViewer";
            PlayerSettings.applicationIdentifier = "com.unitysaba.viewer";
            PlayerSettings.bundleVersion = "1.0.0";
            PlayerSettings.Android.bundleVersionCode = 1;

            EditorUserBuildSettings.SwitchActiveBuildTarget(BuildTargetGroup.Android, BuildTarget.Android);
            EditorUserBuildSettings.androidBuildType = AndroidBuildType.Release;
            EditorUserBuildSettings.development = false;
            EditorUserBuildSettings.allowDebugging = false;

            BuildPlayerOptions buildOptions = new BuildPlayerOptions
            {
                scenes = scenes,
                locationPathName = outputPath,
                targetGroup = BuildTargetGroup.Android,
                target = BuildTarget.Android,
                options = BuildOptions.None
            };

            BuildReport report = BuildPipeline.BuildPlayer(buildOptions);
            BuildSummary summary = report.summary;

            if (summary.result == BuildResult.Succeeded)
            {
                Debug.Log($"[BuildHelper] Build succeeded: {outputPath}");
                Debug.Log($"[BuildHelper] Size: {summary.totalSize / (1024f * 1024f):F2} MB");
                Debug.Log($"[BuildHelper] Time: {summary.totalTime.TotalSeconds:F1}s");
            }
            else
            {
                Debug.LogError($"[BuildHelper] Build failed: {summary.result}");
                foreach (var step in report.steps)
                {
                    if (step.messages != null)
                    {
                        foreach (var msg in step.messages)
                        {
                            if (msg.type == LogType.Error || msg.type == LogType.Exception)
                            {
                                Debug.LogError($"[BuildHelper] {step.name}: {msg.content}");
                            }
                        }
                    }
                }
            }
        }

        [MenuItem("SabaViewer/Build Android APK (Development)")]
        public static void BuildAndroidDevelopment()
        {
            string[] scenes = GetEnabledScenes();

            if (scenes.Length == 0)
            {
                Debug.LogError("[BuildHelper] No enabled scenes found in build settings");
                return;
            }

            string outputPath = $"{BuildPath}/SabaViewer_Dev.apk";

            PlayerSettings.SetScriptingBackend(BuildTargetGroup.Android, ScriptingImplementation.IL2CPP);
            PlayerSettings.Android.targetArchitectures = AndroidArchitecture.ARM64;

            EditorUserBuildSettings.SwitchActiveBuildTarget(BuildTargetGroup.Android, BuildTarget.Android);
            EditorUserBuildSettings.androidBuildType = AndroidBuildType.Development;
            EditorUserBuildSettings.development = true;
            EditorUserBuildSettings.allowDebugging = true;

            BuildPlayerOptions buildOptions = new BuildPlayerOptions
            {
                scenes = scenes,
                locationPathName = outputPath,
                targetGroup = BuildTargetGroup.Android,
                target = BuildTarget.Android,
                options = BuildOptions.Development | BuildOptions.AllowDebugging
            };

            BuildReport report = BuildPipeline.BuildPlayer(buildOptions);
            BuildSummary summary = report.summary;

            if (summary.result == BuildResult.Succeeded)
            {
                Debug.Log($"[BuildHelper] Development build succeeded: {outputPath}");
            }
            else
            {
                Debug.LogError($"[BuildHelper] Development build failed: {summary.result}");
            }
        }

        [MenuItem("SabaViewer/Open Build Folder")]
        public static void OpenBuildFolder()
        {
            if (!System.IO.Directory.Exists(BuildPath))
            {
                System.IO.Directory.CreateDirectory(BuildPath);
            }
            EditorUtility.RevealInFinder(BuildPath);
        }

        [MenuItem("SabaViewer/Clear Build Cache")]
        public static void ClearBuildCache()
        {
            if (System.IO.Directory.Exists("Library/BuildCache"))
            {
                System.IO.Directory.Delete("Library/BuildCache", true);
                Debug.Log("[BuildHelper] Build cache cleared");
            }

            if (System.IO.Directory.Exists("Library/Bee"))
            {
                System.IO.Directory.Delete("Library/Bee", true);
                Debug.Log("[BuildHelper] Bee cache cleared");
            }
        }

        private static string[] GetEnabledScenes()
        {
            return EditorBuildSettingsScene.GetActiveSceneList(EditorBuildSettings.scenes);
        }
    }
}