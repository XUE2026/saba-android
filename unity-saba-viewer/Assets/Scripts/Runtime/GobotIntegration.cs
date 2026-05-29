using System;
using System.Collections.Generic;
using System.IO;
using UnityEngine;

namespace Saba.MMDViewer
{
    [Serializable]
    public class GobotActionData
    {
        public string version = "1.0";
        public string modelName;
        public List<GobotBoneFrame> boneFrames = new List<GobotBoneFrame>();
        public float duration;
        public int frameCount;
    }

    [Serializable]
    public class GobotBoneFrame
    {
        public string boneName;
        public float[] position = new float[3];
        public float[] rotation = new float[4];
        public float timestamp;
    }

    [Serializable]
    public class GobotSceneData
    {
        public string version = "1.0";
        public GobotCameraData camera;
        public GobotEnvironmentData environment;
        public List<GobotPrimitiveData> primitives = new List<GobotPrimitiveData>();
    }

    [Serializable]
    public class GobotCameraData
    {
        public float[] position = new float[3];
        public float[] rotation = new float[4];
        public float distance;
        public float fov;
    }

    [Serializable]
    public class GobotEnvironmentData
    {
        public float[] sunDirection = new float[3];
        public float sunIntensity;
        public string groundType;
        public float[] ambientColor = new float[3];
    }

    [Serializable]
    public class GobotPrimitiveData
    {
        public string type;
        public float[] position = new float[3];
        public float[] scale = new float[3];
        public float[] color = new float[3];
    }

    [Serializable]
    public class GobotProjectData
    {
        public string version = "1.0";
        public string name;
        public GobotActionData action;
        public GobotSceneData scene;
        public string filterType;
        public float filterIntensity;
    }

    public class GobotIntegration : MonoBehaviour
    {
        [Header("References")]
        [SerializeField] private ModelLoader modelLoader;
        [SerializeField] private CameraController cameraController;
        [SerializeField] private FilterManager filterManager;
        [SerializeField] private EnvironmentManager environmentManager;
        [SerializeField] private BoneController boneController;
        [SerializeField] private RecordingManager recordingManager;

        public bool LoadGobotAction(string path)
        {
            try
            {
                if (!File.Exists(path))
                {
                    Debug.LogError($"[GobotIntegration] Action file not found: {path}");
                    return false;
                }

                string json = File.ReadAllText(path);
                GobotActionData actionData = JsonUtility.FromJson<GobotActionData>(json);

                if (actionData == null)
                {
                    Debug.LogError("[GobotIntegration] Failed to parse GoBot action file.");
                    return false;
                }

                ApplyActionData(actionData);
                Debug.Log($"[GobotIntegration] Loaded GoBot action: {actionData.modelName}, {actionData.frameCount} frames.");
                return true;
            }
            catch (Exception e)
            {
                Debug.LogError($"[GobotIntegration] Failed to load GoBot action: {e.Message}");
                return false;
            }
        }

        public bool LoadGobotScene(string path)
        {
            try
            {
                if (!File.Exists(path))
                {
                    Debug.LogError($"[GobotIntegration] Scene file not found: {path}");
                    return false;
                }

                string json = File.ReadAllText(path);
                GobotSceneData sceneData = JsonUtility.FromJson<GobotSceneData>(json);

                if (sceneData == null)
                {
                    Debug.LogError("[GobotIntegration] Failed to parse GoBot scene file.");
                    return false;
                }

                ApplySceneData(sceneData);
                Debug.Log("[GobotIntegration] Loaded GoBot scene.");
                return true;
            }
            catch (Exception e)
            {
                Debug.LogError($"[GobotIntegration] Failed to load GoBot scene: {e.Message}");
                return false;
            }
        }

        public bool LoadGobotProject(string path)
        {
            try
            {
                if (!File.Exists(path))
                {
                    Debug.LogError($"[GobotIntegration] Project file not found: {path}");
                    return false;
                }

                string json = File.ReadAllText(path);
                GobotProjectData projectData = JsonUtility.FromJson<GobotProjectData>(json);

                if (projectData == null)
                {
                    Debug.LogError("[GobotIntegration] Failed to parse GoBot project file.");
                    return false;
                }

                if (projectData.scene != null)
                {
                    ApplySceneData(projectData.scene);
                }

                if (projectData.action != null)
                {
                    ApplyActionData(projectData.action);
                }

                if (!string.IsNullOrEmpty(projectData.filterType) && filterManager != null)
                {
                    if (Enum.TryParse(projectData.filterType, out FilterType filterType))
                    {
                        filterManager.ApplyFilter(filterType);
                        filterManager.SetIntensity(projectData.filterIntensity);
                    }
                }

                Debug.Log($"[GobotIntegration] Loaded GoBot project: {projectData.name}");
                return true;
            }
            catch (Exception e)
            {
                Debug.LogError($"[GobotIntegration] Failed to load GoBot project: {e.Message}");
                return false;
            }
        }

        public bool ExportGobotAction(string path)
        {
            try
            {
                if (recordingManager == null || recordingManager.GetSegmentCount() == 0)
                {
                    Debug.LogWarning("[GobotIntegration] No recording data to export.");
                    return false;
                }

                GobotActionData actionData = new GobotActionData();

                if (modelLoader != null && modelLoader.GetModelRoot() != null)
                {
                    actionData.modelName = modelLoader.GetModelRoot().name;
                }

                List<RecordingManager.RecordSegment> segments = recordingManager.GetSegments();
                foreach (var segment in segments)
                {
                    foreach (var frame in segment.frames)
                    {
                        GobotBoneFrame gobotFrame = new GobotBoneFrame
                        {
                            boneName = frame.boneName,
                            position = new float[] { frame.localPosition.x, frame.localPosition.y, frame.localPosition.z },
                            rotation = new float[] { frame.localRotation.x, frame.localRotation.y, frame.localRotation.z, frame.localRotation.w },
                            timestamp = frame.timestamp
                        };
                        actionData.boneFrames.Add(gobotFrame);
                    }
                }

                actionData.frameCount = actionData.boneFrames.Count;
                if (actionData.frameCount > 0)
                {
                    actionData.duration = actionData.boneFrames[actionData.frameCount - 1].timestamp -
                                         actionData.boneFrames[0].timestamp;
                }

                string json = JsonUtility.ToJson(actionData, true);
                File.WriteAllText(path, json);

                Debug.Log($"[GobotIntegration] Exported GoBot action to: {path}");
                return true;
            }
            catch (Exception e)
            {
                Debug.LogError($"[GobotIntegration] Failed to export GoBot action: {e.Message}");
                return false;
            }
        }

        public bool ExportGobotScene(string path)
        {
            try
            {
                GobotSceneData sceneData = new GobotSceneData();

                if (cameraController != null)
                {
                    sceneData.camera = new GobotCameraData();
                    Transform camTransform = Camera.main?.transform;
                    if (camTransform != null)
                    {
                        sceneData.camera.position = new float[] {
                            camTransform.position.x, camTransform.position.y, camTransform.position.z
                        };
                        sceneData.camera.rotation = new float[] {
                            camTransform.rotation.x, camTransform.rotation.y,
                            camTransform.rotation.z, camTransform.rotation.w
                        };
                        sceneData.camera.distance = cameraController.GetDistance();
                        sceneData.camera.fov = Camera.main.fieldOfView;
                    }
                }

                if (environmentManager != null)
                {
                    sceneData.environment = new GobotEnvironmentData();
                    Vector3 sunDir = environmentManager.GetSunDirection();
                    sceneData.environment.sunDirection = new float[] { sunDir.x, sunDir.y, sunDir.z };
                    sceneData.environment.sunIntensity = environmentManager.GetSunIntensity();
                    sceneData.environment.groundType = environmentManager.GetGroundType().ToString();
                }

                string json = JsonUtility.ToJson(sceneData, true);
                File.WriteAllText(path, json);

                Debug.Log($"[GobotIntegration] Exported GoBot scene to: {path}");
                return true;
            }
            catch (Exception e)
            {
                Debug.LogError($"[GobotIntegration] Failed to export GoBot scene: {e.Message}");
                return false;
            }
        }

        public bool ExportGobotProject(string path)
        {
            try
            {
                GobotProjectData projectData = new GobotProjectData
                {
                    name = modelLoader != null && modelLoader.GetModelRoot() != null
                        ? modelLoader.GetModelRoot().name
                        : "Untitled Project"
                };

                string basePath = Path.GetDirectoryName(path);
                string fileNameWithoutExt = Path.GetFileNameWithoutExtension(path);

                if (recordingManager != null && recordingManager.GetSegmentCount() > 0)
                {
                    string actionPath = Path.Combine(basePath, fileNameWithoutExt + "_action.json");
                    ExportGobotAction(actionPath);
                }

                string scenePath = Path.Combine(basePath, fileNameWithoutExt + "_scene.json");
                ExportGobotScene(scenePath);

                if (filterManager != null)
                {
                    projectData.filterType = filterManager.GetCurrentFilter().ToString();
                    projectData.filterIntensity = filterManager.GetIntensity();
                }

                string json = JsonUtility.ToJson(projectData, true);
                File.WriteAllText(path, json);

                Debug.Log($"[GobotIntegration] Exported GoBot project to: {path}");
                return true;
            }
            catch (Exception e)
            {
                Debug.LogError($"[GobotIntegration] Failed to export GoBot project: {e.Message}");
                return false;
            }
        }

        private void ApplyActionData(GobotActionData actionData)
        {
            if (boneController == null || actionData.boneFrames.Count == 0) return;

            foreach (GobotBoneFrame frame in actionData.boneFrames)
            {
                if (!string.IsNullOrEmpty(frame.boneName))
                {
                    boneController.SelectBone(frame.boneName);
                    Vector3 position = new Vector3(frame.position[0], frame.position[1], frame.position[2]);
                    Quaternion rotation = new Quaternion(frame.rotation[0], frame.rotation[1],
                        frame.rotation[2], frame.rotation[3]);
                    boneController.SetBoneRotation(rotation);
                }
            }
        }

        private void ApplySceneData(GobotSceneData sceneData)
        {
            if (sceneData.camera != null && cameraController != null)
            {
                Vector3 camPos = new Vector3(
                    sceneData.camera.position[0],
                    sceneData.camera.position[1],
                    sceneData.camera.position[2]);

                Quaternion camRot = new Quaternion(
                    sceneData.camera.rotation[0],
                    sceneData.camera.rotation[1],
                    sceneData.camera.rotation[2],
                    sceneData.camera.rotation[3]);

                Camera.main.transform.position = camPos;
                Camera.main.transform.rotation = camRot;

                if (sceneData.camera.distance > 0)
                {
                    cameraController.SetDistance(sceneData.camera.distance);
                }
            }

            if (sceneData.environment != null && environmentManager != null)
            {
                Vector3 sunDir = new Vector3(
                    sceneData.environment.sunDirection[0],
                    sceneData.environment.sunDirection[1],
                    sceneData.environment.sunDirection[2]);
                environmentManager.SetSunDirection(sunDir);
                environmentManager.SetSunIntensity(sceneData.environment.sunIntensity);

                if (Enum.TryParse(sceneData.environment.groundType, out GroundType groundType))
                {
                    environmentManager.SetGroundType(groundType);
                }
            }

            foreach (GobotPrimitiveData primitive in sceneData.primitives)
            {
                PrimitiveType type = primitive.type switch
                {
                    "Cube" => PrimitiveType.Cube,
                    "Sphere" => PrimitiveType.Sphere,
                    "Capsule" => PrimitiveType.Capsule,
                    "Cylinder" => PrimitiveType.Cylinder,
                    _ => PrimitiveType.Cube
                };

                Vector3 pos = new Vector3(primitive.position[0], primitive.position[1], primitive.position[2]);
                Vector3 scale = new Vector3(primitive.scale[0], primitive.scale[1], primitive.scale[2]);
                Color color = new Color(primitive.color[0], primitive.color[1], primitive.color[2]);

                environmentManager.AddPrimitive(type, pos, scale, color);
            }
        }
    }
}