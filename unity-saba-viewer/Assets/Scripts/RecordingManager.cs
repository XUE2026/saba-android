using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using UnityEngine;

namespace UnitySabaViewer
{
    [Serializable]
    public struct RecordedFrame
    {
        public float time;
        public Vector3 position;
        public Quaternion rotation;
        public Dictionary<string, BonePose> bonePoses;

        public RecordedFrame(float time, Vector3 pos, Quaternion rot)
        {
            this.time = time;
            this.position = pos;
            this.rotation = rot;
            this.bonePoses = new Dictionary<string, BonePose>();
        }
    }

    [Serializable]
    public struct BonePose
    {
        public Vector3 position;
        public Quaternion rotation;

        public BonePose(Vector3 pos, Quaternion rot)
        {
            position = pos;
            rotation = rot;
        }
    }

    public class RecordingManager : MonoBehaviour
    {
        public static RecordingManager Instance { get; private set; }

        private bool _isRecording;
        private float _recordStartTime;
        private List<RecordedFrame> _recordedFrames;
        private BoneController _boneController;
        private CameraController _cameraController;
        private float _recordInterval = 1f / 30f;
        private float _lastRecordTime;

        public bool IsRecording => _isRecording;
        public int FrameCount => _recordedFrames != null ? _recordedFrames.Count : 0;
        public float RecordDuration => _isRecording ? Time.time - _recordStartTime : 0f;

        private void Awake()
        {
            if (Instance != null && Instance != this)
            {
                Destroy(this);
                return;
            }
            Instance = this;

            _recordedFrames = new List<RecordedFrame>();
        }

        private void Start()
        {
            _boneController = FindObjectOfType<BoneController>();
            _cameraController = FindObjectOfType<CameraController>();
        }

        private void Update()
        {
            if (!_isRecording) return;

            if (Time.time - _lastRecordTime >= _recordInterval)
            {
                CaptureFrame();
                _lastRecordTime = Time.time;
            }
        }

        private void CaptureFrame()
        {
            float currentTime = Time.time - _recordStartTime;
            Vector3 camPos = Vector3.zero;
            Quaternion camRot = Quaternion.identity;

            if (_cameraController != null)
            {
                camPos = _cameraController.transform.position;
                camRot = _cameraController.transform.rotation;
            }

            RecordedFrame frame = new RecordedFrame(currentTime, camPos, camRot);

            if (_boneController != null)
            {
                string[] boneNames = _boneController.GetBoneList();
                foreach (string boneName in boneNames)
                {
                    Transform bone = _boneController.GetBone(boneName);
                    if (bone != null)
                    {
                        frame.bonePoses[boneName] = new BonePose(bone.localPosition, bone.localRotation);
                    }
                }
            }

            _recordedFrames.Add(frame);
        }

        public void StartRecording()
        {
            if (_isRecording)
            {
                Debug.LogWarning("[RecordingManager] Already recording");
                return;
            }

            _isRecording = true;
            _recordStartTime = Time.time;
            _lastRecordTime = Time.time;
            _recordedFrames.Clear();

            Debug.Log("[RecordingManager] Recording started");
        }

        public void StopRecording()
        {
            if (!_isRecording)
            {
                Debug.LogWarning("[RecordingManager] Not recording");
                return;
            }

            _isRecording = false;
            Debug.Log($"[RecordingManager] Recording stopped. Frames: {_recordedFrames.Count}");
        }

        public void ClearRecording()
        {
            _recordedFrames.Clear();
            Debug.Log("[RecordingManager] Recording cleared");
        }

        public RecordedFrame[] GetRecordedFrames()
        {
            return _recordedFrames.ToArray();
        }

        public void ExportVMD(string path)
        {
            if (_recordedFrames.Count == 0)
            {
                Debug.LogWarning("[RecordingManager] No frames to export");
                return;
            }

            try
            {
                using (BinaryWriter writer = new BinaryWriter(File.Open(path, FileMode.Create)))
                {
                    byte[] header = Encoding.ASCII.GetBytes("Vocaloid Motion Data 0002");
                    writer.Write(header);
                    writer.Write(new byte[42 - header.Length]);

                    string modelName = "SabaViewer_Export";
                    byte[] modelNameBytes = Encoding.ASCII.GetBytes(modelName);
                    writer.Write(modelNameBytes);
                    writer.Write(new byte[20 - modelNameBytes.Length]);

                    uint boneFrameCount = 0;
                    foreach (var frame in _recordedFrames)
                    {
                        boneFrameCount += (uint)frame.bonePoses.Count;
                    }
                    writer.Write(boneFrameCount);

                    foreach (var frame in _recordedFrames)
                    {
                        foreach (var kvp in frame.bonePoses)
                        {
                            byte[] boneNameBytes = Encoding.ASCII.GetBytes(kvp.Key.PadRight(15, '\0').Substring(0, 15));
                            writer.Write(boneNameBytes);

                            uint frameIndex = (uint)(frame.time * 30f);
                            writer.Write(frameIndex);

                            writer.Write(kvp.Value.position.x);
                            writer.Write(kvp.Value.position.y);
                            writer.Write(kvp.Value.position.z);
                            writer.Write(kvp.Value.rotation.x);
                            writer.Write(kvp.Value.rotation.y);
                            writer.Write(kvp.Value.rotation.z);
                            writer.Write(kvp.Value.rotation.w);

                            writer.Write(new byte[64]);
                        }
                    }

                    uint cameraFrameCount = (uint)_recordedFrames.Count;
                    writer.Write(cameraFrameCount);

                    foreach (var frame in _recordedFrames)
                    {
                        uint frameIndex = (uint)(frame.time * 30f);
                        writer.Write(frameIndex);
                        writer.Write(frame.position.x);
                        writer.Write(frame.position.y);
                        writer.Write(frame.position.z);
                        writer.Write(frame.rotation.x);
                        writer.Write(frame.rotation.y);
                        writer.Write(frame.rotation.z);
                        writer.Write(frame.rotation.w);
                        writer.Write(45f);
                        writer.Write(0);
                        writer.Write(new byte[24]);
                    }

                    writer.Write((uint)0);
                    writer.Write((uint)0);
                }

                Debug.Log($"[RecordingManager] Exported VMD to: {path}");
            }
            catch (Exception ex)
            {
                Debug.LogError($"[RecordingManager] Failed to export VMD: {ex.Message}");
            }
        }

        public void ExportGobotAction(string path)
        {
            if (_recordedFrames.Count == 0)
            {
                Debug.LogWarning("[RecordingManager] No frames to export");
                return;
            }

            try
            {
                using (StreamWriter writer = new StreamWriter(path, false, Encoding.UTF8))
                {
                    writer.WriteLine("{");
                    writer.WriteLine("  \"format\": \"gobot_action\",");
                    writer.WriteLine("  \"version\": 1.0,");
                    writer.WriteLine("  \"frames\": [");

                    for (int i = 0; i < _recordedFrames.Count; i++)
                    {
                        var frame = _recordedFrames[i];
                        writer.WriteLine("    {");
                        writer.WriteLine($"      \"time\": {frame.time:F3},");
                        writer.WriteLine("      \"camera\": {");
                        writer.WriteLine($"        \"position\": [{frame.position.x:F6}, {frame.position.y:F6}, {frame.position.z:F6}],");
                        writer.WriteLine($"        \"rotation\": [{frame.rotation.x:F6}, {frame.rotation.y:F6}, {frame.rotation.z:F6}, {frame.rotation.w:F6}]");
                        writer.WriteLine("      },");
                        writer.WriteLine("      \"bones\": {");

                        int boneCount = 0;
                        foreach (var kvp in frame.bonePoses)
                        {
                            writer.WriteLine($"        \"{kvp.Key}\": {{");
                            writer.WriteLine($"          \"position\": [{kvp.Value.position.x:F6}, {kvp.Value.position.y:F6}, {kvp.Value.position.z:F6}],");
                            writer.WriteLine($"          \"rotation\": [{kvp.Value.rotation.x:F6}, {kvp.Value.rotation.y:F6}, {kvp.Value.rotation.z:F6}, {kvp.Value.rotation.w:F6}]");
                            boneCount++;
                            writer.Write("        }");
                            if (boneCount < frame.bonePoses.Count)
                            {
                                writer.WriteLine(",");
                            }
                            else
                            {
                                writer.WriteLine();
                            }
                        }

                        writer.WriteLine("      }");
                        writer.Write("    }");
                        if (i < _recordedFrames.Count - 1)
                        {
                            writer.WriteLine(",");
                        }
                        else
                        {
                            writer.WriteLine();
                        }
                    }

                    writer.WriteLine("  ]");
                    writer.WriteLine("}");
                }

                Debug.Log($"[RecordingManager] Exported GoBot Action to: {path}");
            }
            catch (Exception ex)
            {
                Debug.LogError($"[RecordingManager] Failed to export GoBot Action: {ex.Message}");
            }
        }

        private void OnDestroy()
        {
            if (_instance == this)
            {
                _instance = null;
            }
        }
    }
}