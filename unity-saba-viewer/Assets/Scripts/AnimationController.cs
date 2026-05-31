using System.Collections.Generic;
using UnityEngine;

namespace UnitySabaViewer
{
    public class AnimationController : MonoBehaviour
    {
        public enum AnimationState
        {
            Idle,
            Walk,
            Run,
            Jump,
            None
        }

        [Header("Settings")]
        [SerializeField] private float _crossFadeDuration = 0.3f;

        private BoneController _boneController;
        private AnimationState _currentState = AnimationState.None;
        private AnimationState _previousState = AnimationState.None;
        private float _transitionProgress = 1f;
        private bool _isPlaying;

        private Dictionary<string, List<BoneKeyframe>> _idleCurves;
        private Dictionary<string, List<BoneKeyframe>> _walkCurves;
        private Dictionary<string, List<BoneKeyframe>> _runCurves;
        private Dictionary<string, List<BoneKeyframe>> _jumpCurves;

        private Dictionary<string, BoneKeyframe> _currentFrame = new Dictionary<string, BoneKeyframe>();
        private Dictionary<string, BoneKeyframe> _targetFrame = new Dictionary<string, BoneKeyframe>();
        private Dictionary<string, BoneKeyframe> _fromFrame = new Dictionary<string, BoneKeyframe>();

        public bool IsPlaying => _isPlaying;
        public AnimationState CurrentState => _currentState;

        public struct BoneKeyframe
        {
            public Vector3 position;
            public Quaternion rotation;

            public BoneKeyframe(Vector3 pos, Quaternion rot)
            {
                position = pos;
                rotation = rot;
            }
        }

        private void Awake()
        {
            _boneController = GetComponent<BoneController>();
            if (_boneController == null)
            {
                _boneController = GetComponentInChildren<BoneController>();
            }

            BuildAnimationCurves();
        }

        private void BuildAnimationCurves()
        {
            _idleCurves = new Dictionary<string, List<BoneKeyframe>>();
            _walkCurves = new Dictionary<string, List<BoneKeyframe>>();
            _runCurves = new Dictionary<string, List<BoneKeyframe>>();
            _jumpCurves = new Dictionary<string, List<BoneKeyframe>>();

            string[] mmdBones = new string[]
            {
                "センター", "グルーブ", "腰", "上半身", "上半身2",
                "首", "頭", "左腕", "左手首", "左指1", "左指2",
                "右腕", "右手首", "右指1", "右指2",
                "左足", "左ひざ", "左足首", "右足", "右ひざ", "右足首",
                "左目", "右目", "まゆL", "まゆR", "あご"
            };

            foreach (string bone in mmdBones)
            {
                _idleCurves[bone] = GenerateIdleCurve(bone);
                _walkCurves[bone] = GenerateWalkCurve(bone);
                _runCurves[bone] = GenerateRunCurve(bone);
                _jumpCurves[bone] = GenerateJumpCurve(bone);
            }
        }

        private List<BoneKeyframe> GenerateIdleCurve(string boneName)
        {
            List<BoneKeyframe> curve = new List<BoneKeyframe>();
            float breathCycle = 2.0f;
            int frameCount = Mathf.RoundToInt(breathCycle * 60f);

            for (int i = 0; i <= frameCount; i++)
            {
                float t = (float)i / frameCount;
                float breath = Mathf.Sin(t * Mathf.PI * 2) * 0.3f;
                Vector3 pos = Vector3.zero;
                Quaternion rot = Quaternion.identity;

                switch (boneName)
                {
                    case "上半身":
                        rot = Quaternion.Euler(breath * 0.5f, 0, 0);
                        break;
                    case "頭":
                        rot = Quaternion.Euler(0, 0, breath * 0.3f);
                        break;
                }

                curve.Add(new BoneKeyframe(pos, rot));
            }

            return curve;
        }

        private List<BoneKeyframe> GenerateWalkCurve(string boneName)
        {
            List<BoneKeyframe> curve = new List<BoneKeyframe>();
            float cycle = 0.6f;
            int frameCount = Mathf.RoundToInt(cycle * 60f);

            for (int i = 0; i <= frameCount; i++)
            {
                float t = (float)i / frameCount;
                float angle = t * Mathf.PI * 2;
                Vector3 pos = Vector3.zero;
                Quaternion rot = Quaternion.identity;

                switch (boneName)
                {
                    case "左足":
                        pos = new Vector3(0, Mathf.Sin(angle) * 0.1f, 0);
                        rot = Quaternion.Euler(Mathf.Sin(angle) * 20f, 0, 0);
                        break;
                    case "右足":
                        pos = new Vector3(0, Mathf.Sin(angle + Mathf.PI) * 0.1f, 0);
                        rot = Quaternion.Euler(Mathf.Sin(angle + Mathf.PI) * 20f, 0, 0);
                        break;
                    case "左腕":
                        rot = Quaternion.Euler(0, 0, Mathf.Sin(angle + Mathf.PI) * 15f);
                        break;
                    case "右腕":
                        rot = Quaternion.Euler(0, 0, Mathf.Sin(angle) * 15f);
                        break;
                    case "上半身":
                        rot = Quaternion.Euler(0, Mathf.Sin(angle) * 3f, 0);
                        break;
                }

                curve.Add(new BoneKeyframe(pos, rot));
            }

            return curve;
        }

        private List<BoneKeyframe> GenerateRunCurve(string boneName)
        {
            List<BoneKeyframe> curve = new List<BoneKeyframe>();
            float cycle = 0.4f;
            int frameCount = Mathf.RoundToInt(cycle * 60f);

            for (int i = 0; i <= frameCount; i++)
            {
                float t = (float)i / frameCount;
                float angle = t * Mathf.PI * 2;
                Vector3 pos = Vector3.zero;
                Quaternion rot = Quaternion.identity;

                switch (boneName)
                {
                    case "左足":
                        pos = new Vector3(0, Mathf.Sin(angle) * 0.2f, 0);
                        rot = Quaternion.Euler(Mathf.Sin(angle) * 35f, 0, 0);
                        break;
                    case "右足":
                        pos = new Vector3(0, Mathf.Sin(angle + Mathf.PI) * 0.2f, 0);
                        rot = Quaternion.Euler(Mathf.Sin(angle + Mathf.PI) * 35f, 0, 0);
                        break;
                    case "左腕":
                        rot = Quaternion.Euler(0, 0, Mathf.Sin(angle + Mathf.PI) * 30f);
                        break;
                    case "右腕":
                        rot = Quaternion.Euler(0, 0, Mathf.Sin(angle) * 30f);
                        break;
                    case "上半身":
                        rot = Quaternion.Euler(Mathf.Abs(Mathf.Sin(angle)) * 5f, 0, 0);
                        break;
                }

                curve.Add(new BoneKeyframe(pos, rot));
            }

            return curve;
        }

        private List<BoneKeyframe> GenerateJumpCurve(string boneName)
        {
            List<BoneKeyframe> curve = new List<BoneKeyframe>();
            float duration = 0.8f;
            int frameCount = Mathf.RoundToInt(duration * 60f);

            for (int i = 0; i <= frameCount; i++)
            {
                float t = (float)i / frameCount;
                float height = Mathf.Sin(t * Mathf.PI);
                Vector3 pos = Vector3.zero;
                Quaternion rot = Quaternion.identity;

                switch (boneName)
                {
                    case "センター":
                        pos = new Vector3(0, height * 0.5f, 0);
                        break;
                    case "左足":
                        rot = Quaternion.Euler(-30f * height, 0, 0);
                        break;
                    case "右足":
                        rot = Quaternion.Euler(-30f * height, 0, 0);
                        break;
                    case "左腕":
                        rot = Quaternion.Euler(0, 0, -45f * height);
                        break;
                    case "右腕":
                        rot = Quaternion.Euler(0, 0, 45f * height);
                        break;
                }

                curve.Add(new BoneKeyframe(pos, rot));
            }

            return curve;
        }

        private void Update()
        {
            if (!_isPlaying) return;

            if (_currentState == AnimationState.None) return;

            if (_transitionProgress < 1f)
            {
                _transitionProgress += Time.deltaTime / _crossFadeDuration;
            }

            float time = Time.time;
            AnimateBones(_currentState, time);

            ApplyBoneAnimations();
        }

        private void AnimateBones(AnimationState state, float time)
        {
            Dictionary<string, List<BoneKeyframe>> curves = GetCurves(state);
            if (curves == null) return;

            string[] boneNames = GetBoneList();
            foreach (string boneName in boneNames)
            {
                if (!curves.ContainsKey(boneName)) continue;

                List<BoneKeyframe> curve = curves[boneName];
                if (curve.Count == 0) continue;

                float animSpeed = 1f;
                switch (state)
                {
                    case AnimationState.Walk: animSpeed = 1f; break;
                    case AnimationState.Run: animSpeed = 2f; break;
                    case AnimationState.Idle: animSpeed = 0.5f; break;
                    case AnimationState.Jump: animSpeed = 1f; break;
                }

                float t = (time * animSpeed) % 1f;
                int frameIndex = Mathf.FloorToInt(t * (curve.Count - 1));
                frameIndex = Mathf.Clamp(frameIndex, 0, curve.Count - 1);

                BoneKeyframe keyframe = curve[frameIndex];

                if (_transitionProgress < 1f && _previousState != AnimationState.None)
                {
                    BoneKeyframe fromFrame = GetFromFrame(boneName, _previousState, time);
                    _currentFrame[boneName] = BoneKeyframe.Lerp(fromFrame, keyframe, _transitionProgress);
                }
                else
                {
                    _currentFrame[boneName] = keyframe;
                }
            }
        }

        private BoneKeyframe GetFromFrame(string boneName, AnimationState state, float time)
        {
            Dictionary<string, List<BoneKeyframe>> curves = GetCurves(state);
            if (curves == null || !curves.ContainsKey(boneName) || curves[boneName].Count == 0)
            {
                return new BoneKeyframe(Vector3.zero, Quaternion.identity);
            }

            List<BoneKeyframe> curve = curves[boneName];
            float t = time % 1f;
            int frameIndex = Mathf.FloorToInt(t * (curve.Count - 1));
            frameIndex = Mathf.Clamp(frameIndex, 0, curve.Count - 1);
            return curve[frameIndex];
        }

        private string[] GetBoneList()
        {
            if (_boneController != null)
            {
                return _boneController.GetBoneList();
            }
            return new string[]
            {
                "センター", "上半身", "頭", "左腕", "右腕",
                "左足", "右足", "左ひざ", "右ひざ"
            };
        }

        private void ApplyBoneAnimations()
        {
            if (_boneController == null) return;

            foreach (var kvp in _currentFrame)
            {
                if (_boneController.HasBone(kvp.Key))
                {
                    _boneController.TranslateBone(kvp.Key, kvp.Value.position);
                    _boneController.RotateBone(kvp.Key, kvp.Value.rotation);
                }
            }
        }

        private Dictionary<string, List<BoneKeyframe>> GetCurves(AnimationState state)
        {
            switch (state)
            {
                case AnimationState.Idle: return _idleCurves;
                case AnimationState.Walk: return _walkCurves;
                case AnimationState.Run: return _runCurves;
                case AnimationState.Jump: return _jumpCurves;
                default: return null;
            }
        }

        public void PlayIdle()
        {
            CrossFade(AnimationState.Idle, _crossFadeDuration);
        }

        public void PlayWalk()
        {
            CrossFade(AnimationState.Walk, _crossFadeDuration);
        }

        public void PlayRun()
        {
            CrossFade(AnimationState.Run, _crossFadeDuration);
        }

        public void PlayJump()
        {
            CrossFade(AnimationState.Jump, _crossFadeDuration);
        }

        public void Stop()
        {
            _isPlaying = false;
            _currentState = AnimationState.None;
            ResetBones();
        }

        public void CrossFade(string animName, float time)
        {
            switch (animName.ToLower())
            {
                case "idle":
                    CrossFade(AnimationState.Idle, time);
                    break;
                case "walk":
                    CrossFade(AnimationState.Walk, time);
                    break;
                case "run":
                    CrossFade(AnimationState.Run, time);
                    break;
                case "jump":
                    CrossFade(AnimationState.Jump, time);
                    break;
                default:
                    Debug.LogWarning($"[AnimationController] Unknown animation: {animName}");
                    break;
            }
        }

        public void CrossFade(AnimationState newState, float time)
        {
            if (newState == _currentState) return;

            _previousState = _currentState;
            _currentState = newState;
            _transitionProgress = 0f;
            _crossFadeDuration = Mathf.Max(0.1f, time);
            _isPlaying = true;
        }

        private void ResetBones()
        {
            if (_boneController != null)
            {
                _boneController.ResetAllBones();
            }
            _currentFrame.Clear();
        }
    }
}