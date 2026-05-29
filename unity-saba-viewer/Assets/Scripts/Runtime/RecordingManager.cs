using System;
using System.Collections.Generic;
using System.IO;
using UnityEngine;

namespace Saba.MMDViewer
{
    [Serializable]
    public class BoneTransformRecord
    {
        public string boneName;
        public Vector3 localPosition;
        public Quaternion localRotation;
        public float timestamp;

        public BoneTransformRecord(string boneName, Vector3 localPosition, Quaternion localRotation, float timestamp)
        {
            this.boneName = boneName;
            this.localPosition = localPosition;
            this.localRotation = localRotation;
            this.timestamp = timestamp;
        }
    }

    [Serializable]
    public class RecordSegment
    {
        public List<BoneTransformRecord> frames = new List<BoneTransformRecord>();
        public float duration;
        public float gapTime = 0.5f;
        public int frameCount;

        public void AddFrame(BoneTransformRecord record)
        {
            frames.Add(record);
            frameCount = frames.Count;
            if (frames.Count >= 2)
            {
                duration = frames[frames.Count - 1].timestamp - frames[0].timestamp;
            }
        }
    }

    [Serializable]
    public class RecordingData
    {
        public List<RecordSegment> segments = new List<RecordSegment>();
        public float totalDuration;
        public int totalFrames;
        public DateTime recordedAt;

        public RecordingData()
        {
            recordedAt = DateTime.Now;
        }
    }

    public class RecordingManager : MonoBehaviour
    {
        [Header("Recording Settings")]
        [SerializeField] private BoneController boneController;
        [SerializeField] private float recordingInterval = 0.033f;
        [SerializeField] private float motionPauseThreshold = 0.001f;
        [SerializeField] private float pauseDurationThreshold = 1.0f;
        [SerializeField] private int maxSegmentFrames = 5000;

        [Header("Playback Settings")]
        [SerializeField] private float playbackSpeed = 1f;

        private RecordingData recordingData;
        private RecordSegment currentSegment;
        private bool isRecording;
        private bool isPlaying;
        private float recordingTimer;
        private float pauseTimer;
        private float playbackTimer;
        private int currentSegmentIndex;
        private int currentFrameIndex;
        private Vector3 lastBonePosition;
        private Quaternion lastBoneRotation;
        private Dictionary<Transform, Vector3> initialPositions = new Dictionary<Transform, Vector3>();
        private Dictionary<Transform, Quaternion> initialRotations = new Dictionary<Transform, Quaternion>();

        private List<BoneTransformRecord> playbackCache = new List<BoneTransformRecord>();

        public event Action OnRecordingStarted;
        public event Action OnRecordingStopped;
        public event Action OnPlaybackStarted;
        public event Action OnPlaybackStopped;

        private void Awake()
        {
            recordingData = new RecordingData();
        }

        public void StartRecording()
        {
            if (boneController == null || !boneController.HasAnimator())
            {
                Debug.LogWarning("[RecordingManager] No valid bone controller for recording.");
                return;
            }

            isRecording = true;
            recordingTimer = 0f;
            pauseTimer = 0f;
            currentSegment = new RecordSegment();

            recordingData = new RecordingData();
            recordingData.segments.Clear();

            SaveInitialTransforms();

            OnRecordingStarted?.Invoke();

            Debug.Log("[RecordingManager] Recording started.");
        }

        public void StopRecording()
        {
            if (!isRecording) return;

            if (currentSegment != null && currentSegment.frames.Count > 0)
            {
                recordingData.segments.Add(currentSegment);
            }

            isRecording = false;
            recordingTimer = 0f;

            ComputeRecordingStats();

            OnRecordingStopped?.Invoke();

            Debug.Log($"[RecordingManager] Recording stopped. {recordingData.totalFrames} frames in {recordingData.segments.Count} segments.");
        }

        public bool IsRecording()
        {
            return isRecording;
        }

        public bool IsPlaying()
        {
            return isPlaying;
        }

        public List<RecordSegment> GetSegments()
        {
            if (recordingData == null) return new List<RecordSegment>();
            return recordingData.segments;
        }

        public RecordingData GetRecordingData()
        {
            return recordingData;
        }

        public int GetSegmentCount()
        {
            return recordingData?.segments.Count ?? 0;
        }

        public void SetSegmentGap(int index, float gap)
        {
            if (recordingData == null || index < 0 || index >= recordingData.segments.Count) return;

            recordingData.segments[index].gapTime = Mathf.Max(0f, gap);
        }

        public float GetSegmentGap(int index)
        {
            if (recordingData == null || index < 0 || index >= recordingData.segments.Count) return 0.5f;
            return recordingData.segments[index].gapTime;
        }

        public void PlayAll()
        {
            if (recordingData == null || recordingData.segments.Count == 0)
            {
                Debug.LogWarning("[RecordingManager] No recorded data to play.");
                return;
            }

            if (isPlaying) return;

            isPlaying = true;
            playbackTimer = 0f;
            currentSegmentIndex = 0;
            currentFrameIndex = 0;

            BuildPlaybackCache();

            OnPlaybackStarted?.Invoke();

            Debug.Log("[RecordingManager] Playback started.");
        }

        public void StopPlayback()
        {
            if (!isPlaying) return;

            isPlaying = false;
            playbackTimer = 0f;
            currentSegmentIndex = 0;
            currentFrameIndex = 0;

            RestoreInitialTransforms();

            OnPlaybackStopped?.Invoke();

            Debug.Log("[RecordingManager] Playback stopped.");
        }

        public void ExportAnimation(string path)
        {
            if (recordingData == null || recordingData.segments.Count == 0)
            {
                Debug.LogWarning("[RecordingManager] No recorded data to export.");
                return;
            }

            if (string.IsNullOrEmpty(path))
            {
                path = Path.Combine(Application.persistentDataPath,
                    $"recording_{DateTime.Now:yyyyMMdd_HHmmss}.json");
            }

            try
            {
                string json = JsonUtility.ToJson(recordingData, true);
                File.WriteAllText(path, json);
                Debug.Log($"[RecordingManager] Animation exported to: {path}");
            }
            catch (Exception e)
            {
                Debug.LogError($"[RecordingManager] Failed to export animation: {e.Message}");
            }
        }

        public bool ImportAnimation(string path)
        {
            try
            {
                if (!File.Exists(path))
                {
                    Debug.LogError($"[RecordingManager] File not found: {path}");
                    return false;
                }

                string json = File.ReadAllText(path);
                recordingData = JsonUtility.FromJson<RecordingData>(json);

                if (recordingData == null)
                {
                    Debug.LogError("[RecordingManager] Failed to parse recording data.");
                    return false;
                }

                Debug.Log($"[RecordingManager] Imported {recordingData.segments.Count} segments from {path}");
                return true;
            }
            catch (Exception e)
            {
                Debug.LogError($"[RecordingManager] Failed to import animation: {e.Message}");
                return false;
            }
        }

        public void ClearRecording()
        {
            StopPlayback();
            recordingData = new RecordingData();
            currentSegment = null;
            Debug.Log("[RecordingManager] Recording data cleared.");
        }

        private void Update()
        {
            if (isRecording)
            {
                UpdateRecording();
            }

            if (isPlaying)
            {
                UpdatePlayback();
            }
        }

        private void UpdateRecording()
        {
            recordingTimer += Time.deltaTime;

            if (recordingTimer >= recordingInterval)
            {
                recordingTimer = 0f;

                if (boneController == null) return;

                List<BoneInfo> bones = boneController.GetBoneInfoList();
                bool hasMotion = false;
                float timestamp = Time.time;

                foreach (BoneInfo bone in bones)
                {
                    if (bone == null || !bone.isValid || bone.boneTransform == null) continue;

                    Vector3 posDelta = bone.boneTransform.localPosition - lastBonePosition;
                    float angleDelta = Quaternion.Angle(bone.boneTransform.localRotation, lastBoneRotation);

                    if (posDelta.sqrMagnitude > motionPauseThreshold || angleDelta > 0.1f)
                    {
                        hasMotion = true;
                    }

                    lastBonePosition = bone.boneTransform.localPosition;
                    lastBoneRotation = bone.boneTransform.localRotation;

                    var record = new BoneTransformRecord(
                        bone.name,
                        bone.boneTransform.localPosition,
                        bone.boneTransform.localRotation,
                        timestamp
                    );

                    currentSegment.AddFrame(record);
                }

                if (hasMotion)
                {
                    pauseTimer = 0f;
                }
                else
                {
                    pauseTimer += recordingInterval;
                }

                if (pauseTimer >= pauseDurationThreshold && currentSegment.frames.Count > 0)
                {
                    recordingData.segments.Add(currentSegment);
                    currentSegment = new RecordSegment();
                    pauseTimer = 0f;
                    Debug.Log("[RecordingManager] Auto-split segment due to motion pause.");
                }

                if (currentSegment.frames.Count >= maxSegmentFrames)
                {
                    recordingData.segments.Add(currentSegment);
                    currentSegment = new RecordSegment();
                    Debug.Log("[RecordingManager] Auto-split segment due to max frames.");
                }
            }
        }

        private void UpdatePlayback()
        {
            if (playbackCache.Count == 0)
            {
                StopPlayback();
                return;
            }

            playbackTimer += Time.deltaTime * playbackSpeed;

            if (currentFrameIndex >= playbackCache.Count)
            {
                StopPlayback();
                return;
            }

            BoneTransformRecord record = playbackCache[currentFrameIndex];
            ApplyBoneRecord(record);
            currentFrameIndex++;
        }

        private void BuildPlaybackCache()
        {
            playbackCache.Clear();

            foreach (RecordSegment segment in recordingData.segments)
            {
                playbackCache.AddRange(segment.frames);
            }

            Debug.Log($"[RecordingManager] Playback cache built: {playbackCache.Count} frames.");
        }

        private void ApplyBoneRecord(BoneTransformRecord record)
        {
            if (boneController == null) return;

            List<BoneInfo> bones = boneController.GetBoneInfoList();
            foreach (BoneInfo bone in bones)
            {
                if (bone == null || !bone.isValid || bone.boneTransform == null) continue;

                if (bone.name == record.boneName)
                {
                    bone.boneTransform.localPosition = record.localPosition;
                    bone.boneTransform.localRotation = record.localRotation;
                    break;
                }
            }
        }

        private void SaveInitialTransforms()
        {
            if (boneController == null) return;

            initialPositions.Clear();
            initialRotations.Clear();

            List<BoneInfo> bones = boneController.GetBoneInfoList();
            foreach (BoneInfo bone in bones)
            {
                if (bone != null && bone.boneTransform != null)
                {
                    initialPositions[bone.boneTransform] = bone.boneTransform.localPosition;
                    initialRotations[bone.boneTransform] = bone.boneTransform.localRotation;
                }
            }
        }

        private void RestoreInitialTransforms()
        {
            foreach (var kvp in initialPositions)
            {
                if (kvp.Key != null)
                {
                    kvp.Key.localPosition = kvp.Value;
                }
            }

            foreach (var kvp in initialRotations)
            {
                if (kvp.Key != null)
                {
                    kvp.Key.localRotation = kvp.Value;
                }
            }
        }

        private void ComputeRecordingStats()
        {
            if (recordingData == null) return;

            recordingData.totalFrames = 0;
            recordingData.totalDuration = 0f;

            foreach (RecordSegment segment in recordingData.segments)
            {
                recordingData.totalFrames += segment.frameCount;
                recordingData.totalDuration += segment.duration;
            }
        }

        public void SetPlaybackSpeed(float speed)
        {
            playbackSpeed = Mathf.Max(0.1f, speed);
        }

        public float GetPlaybackSpeed()
        {
            return playbackSpeed;
        }

        private void OnDestroy()
        {
            StopPlayback();
        }
    }
}