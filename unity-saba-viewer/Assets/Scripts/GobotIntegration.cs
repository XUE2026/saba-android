using System;
using System.Collections.Generic;
using System.IO;
using UnityEngine;

namespace UnitySabaViewer
{
    [Serializable]
    public class GobotActionData
    {
        public string format;
        public float version;
        public GobotFrame[] frames;
    }

    [Serializable]
    public class GobotFrame
    {
        public float time;
        public GobotCameraData camera;
        public Dictionary<string, GobotBoneData> bones;
    }

    [Serializable]
    public class GobotCameraData
    {
        public float[] position;
        public float[] rotation;
    }

    [Serializable]
    public class GobotBoneData
    {
        public float[] position;
        public float[] rotation;
    }

    [Serializable]
    public class GobotSceneData
    {
        public string format;
        public float version;
        public GobotSceneEnvironment environment;
        public string[] models;
    }

    [Serializable]
    public class GobotSceneEnvironment
    {
        public float[] sunDirection;
        public float sunIntensity;
        public string groundType;
        public GobotPrimitiveData[] primitives;
    }

    [Serializable]
    public class GobotPrimitiveData
    {
        public string type;
        public float[] position;
        public float[] rotation;
        public float[] scale;
    }

    public class GobotIntegration : MonoBehaviour
    {
        public static GobotIntegration Instance { get; private set; }

        private BoneController _boneController;
        private EnvironmentManager _environmentManager;
        private CameraController _cameraController;

        private GobotActionData _loadedAction;
        private GobotSceneData _loadedScene;
        private float _actionPlaybackTime;
        private bool _isPlayingAction;
        private float _actionDuration;

        public bool IsPlayingAction => _isPlayingAction;
        public float ActionPlaybackTime => _actionPlaybackTime;
        public float ActionDuration => _actionDuration;

        private void Awake()
        {
            if (Instance != null && Instance != this)
            {
                Destroy(this);
                return;
            }
            Instance = this;
        }

        private void Start()
        {
            _boneController = FindObjectOfType<BoneController>();
            _environmentManager = FindObjectOfType<EnvironmentManager>();
            _cameraController = FindObjectOfType<CameraController>();
        }

        private void Update()
        {
            if (!_isPlayingAction || _loadedAction == null) return;

            _actionPlaybackTime += Time.deltaTime;

            if (_actionPlaybackTime >= _actionDuration)
            {
                _isPlayingAction = false;
                return;
            }

            ApplyActionFrame(_actionPlaybackTime);
        }

        public void LoadGobotAction(string path)
        {
            try
            {
                string json = File.ReadAllText(path);
                _loadedAction = JsonUtility.FromJson<GobotActionData>(json);

                if (_loadedAction == null || _loadedAction.frames == null)
                {
                    Debug.LogError("[GobotIntegration] Invalid gobot_action file");
                    return;
                }

                _actionDuration = 0f;
                foreach (var frame in _loadedAction.frames)
                {
                    if (frame.time > _actionDuration)
                    {
                        _actionDuration = frame.time;
                    }
                }

                Debug.Log($"[GobotIntegration] Loaded action: {path}, frames: {_loadedAction.frames.Length}, duration: {_actionDuration}s");
            }
            catch (Exception ex)
            {
                Debug.LogError($"[GobotIntegration] Failed to load action: {ex.Message}");
            }
        }

        public void LoadGobotScene(string path)
        {
            try
            {
                string json = File.ReadAllText(path);
                _loadedScene = JsonUtility.FromJson<GobotSceneData>(json);

                if (_loadedScene == null)
                {
                    Debug.LogError("[GobotIntegration] Invalid gobot_scene file");
                    return;
                }

                if (_loadedScene.environment != null)
                {
                    ApplySceneEnvironment(_loadedScene.environment);
                }

                Debug.Log($"[GobotIntegration] Loaded scene: {path}");
            }
            catch (Exception ex)
            {
                Debug.LogError($"[GobotIntegration] Failed to load scene: {ex.Message}");
            }
        }

        private void ApplySceneEnvironment(GobotSceneEnvironment env)
        {
            if (_environmentManager == null) return;

            if (env.sunDirection != null && env.sunDirection.Length >= 2)
            {
                _environmentManager.SetSunDirection(env.sunDirection[0], env.sunDirection[1]);
            }

            if (env.sunIntensity > 0)
            {
                _environmentManager.SetSunIntensity(env.sunIntensity);
            }

            if (!string.IsNullOrEmpty(env.groundType))
            {
                switch (env.groundType.ToLower())
                {
                    case "grid": _environmentManager.SetGround(0); break;
                    case "checker": _environmentManager.SetGround(1); break;
                    case "grass": _environmentManager.SetGround(2); break;
                    case "dirt": _environmentManager.SetGround(3); break;
                    case "asphalt": _environmentManager.SetGround(4); break;
                }
            }

            if (env.primitives != null)
            {
                _environmentManager.ClearPrimitives();
                foreach (var prim in env.primitives)
                {
                    int typeIndex = 0;
                    switch (prim.type.ToLower())
                    {
                        case "cube": typeIndex = 0; break;
                        case "sphere": typeIndex = 1; break;
                        case "cone": typeIndex = 2; break;
                        case "cylinder": typeIndex = 3; break;
                    }

                    Vector3 pos = Vector3.zero;
                    if (prim.position != null && prim.position.Length >= 3)
                    {
                        pos = new Vector3(prim.position[0], prim.position[1], prim.position[2]);
                    }

                    _environmentManager.AddPrimitive(typeIndex, pos);
                }
            }
        }

        private void ApplyActionFrame(float time)
        {
            if (_loadedAction == null || _loadedAction.frames == null) return;

            GobotFrame prevFrame = null;
            GobotFrame nextFrame = null;

            for (int i = 0; i < _loadedAction.frames.Length; i++)
            {
                if (_loadedAction.frames[i].time <= time)
                {
                    prevFrame = _loadedAction.frames[i];
                }
                if (_loadedAction.frames[i].time >= time && nextFrame == null)
                {
                    nextFrame = _loadedAction.frames[i];
                    break;
                }
            }

            if (prevFrame == null && nextFrame != null)
            {
                prevFrame = nextFrame;
            }
            if (nextFrame == null)
            {
                nextFrame = prevFrame;
            }

            if (prevFrame == null || nextFrame == null) return;

            float t = 0f;
            if (Mathf.Abs(nextFrame.time - prevFrame.time) > 0.001f)
            {
                t = (time - prevFrame.time) / (nextFrame.time - prevFrame.time);
            }

            ApplyCameraFrame(prevFrame.camera, nextFrame.camera, t);
            ApplyBoneFrames(prevFrame.bones, nextFrame.bones, t);
        }

        private void ApplyCameraFrame(GobotCameraData from, GobotCameraData to, float t)
        {
            if (_cameraController == null) return;
            if (from == null || to == null) return;

            if (from.position != null && from.position.Length >= 3 &&
                to.position != null && to.position.Length >= 3)
            {
                Vector3 pos = Vector3.Lerp(
                    new Vector3(from.position[0], from.position[1], from.position[2]),
                    new Vector3(to.position[0], to.position[1], to.position[2]),
                    t
                );
                _cameraController.TargetPosition = pos;
            }

            if (from.rotation != null && from.rotation.Length >= 4 &&
                to.rotation != null && to.rotation.Length >= 4)
            {
                Quaternion rot = Quaternion.Slerp(
                    new Quaternion(from.rotation[0], from.rotation[1], from.rotation[2], from.rotation[3]),
                    new Quaternion(to.rotation[0], to.rotation[1], to.rotation[2], to.rotation[3]),
                    t
                );
                _cameraController.transform.rotation = rot;
            }
        }

        private void ApplyBoneFrames(Dictionary<string, GobotBoneData> from, Dictionary<string, GobotBoneData> to, float t)
        {
            if (_boneController == null) return;
            if (from == null || to == null) return;

            foreach (var kvp in to)
            {
                if (!_boneController.HasBone(kvp.Key)) continue;

                if (from.TryGetValue(kvp.Key, out GobotBoneData fromBone))
                {
                    Vector3 fromPos = fromBone.position != null && fromBone.position.Length >= 3
                        ? new Vector3(fromBone.position[0], fromBone.position[1], fromBone.position[2])
                        : Vector3.zero;

                    Vector3 toPos = kvp.Value.position != null && kvp.Value.position.Length >= 3
                        ? new Vector3(kvp.Value.position[0], kvp.Value.position[1], kvp.Value.position[2])
                        : Vector3.zero;

                    Quaternion fromRot = fromBone.rotation != null && fromBone.rotation.Length >= 4
                        ? new Quaternion(fromBone.rotation[0], fromBone.rotation[1], fromBone.rotation[2], fromBone.rotation[3])
                        : Quaternion.identity;

                    Quaternion toRot = kvp.Value.rotation != null && kvp.Value.rotation.Length >= 4
                        ? new Quaternion(kvp.Value.rotation[0], kvp.Value.rotation[1], kvp.Value.rotation[2], kvp.Value.rotation[3])
                        : Quaternion.identity;

                    _boneController.TranslateBone(kvp.Key, Vector3.Lerp(fromPos, toPos, t));
                    _boneController.RotateBone(kvp.Key, Quaternion.Slerp(fromRot, toRot, t));
                }
            }
        }

        public void ExportGobotProj(string path)
        {
            try
            {
                var projectData = new
                {
                    format = "gobot_project",
                    version = 1.0,
                    exportTime = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss"),
                    description = "Exported from SabaViewer"
                };

                string json = JsonUtility.ToJson(projectData, true);
                File.WriteAllText(path, json);

                Debug.Log($"[GobotIntegration] Exported project to: {path}");
            }
            catch (Exception ex)
            {
                Debug.LogError($"[GobotIntegration] Failed to export project: {ex.Message}");
            }
        }

        public void PlayAction()
        {
            if (_loadedAction == null || _loadedAction.frames == null)
            {
                Debug.LogWarning("[GobotIntegration] No action loaded to play");
                return;
            }

            _actionPlaybackTime = 0f;
            _isPlayingAction = true;
        }

        public void StopAction()
        {
            _isPlayingAction = false;
            _actionPlaybackTime = 0f;
        }

        public void SetActionTime(float time)
        {
            _actionPlaybackTime = Mathf.Clamp(time, 0, _actionDuration);
            ApplyActionFrame(_actionPlaybackTime);
        }
    }
}