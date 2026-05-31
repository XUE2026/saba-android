using System.Collections.Generic;
using UnityEngine;

namespace UnitySabaViewer
{
    public class BoneController : MonoBehaviour
    {
        private Dictionary<string, Transform> _bones = new Dictionary<string, Transform>();
        private Dictionary<string, Vector3> _boneDefaultPositions = new Dictionary<string, Vector3>();
        private Dictionary<string, Quaternion> _boneDefaultRotations = new Dictionary<string, Quaternion>();
        private Transform _bonesContainer;

        public Dictionary<string, Transform> Bones => _bones;
        public int BoneCount => _bones.Count;

        public void Initialize(Transform container)
        {
            _bonesContainer = container;
            _bones.Clear();
            _boneDefaultPositions.Clear();
            _boneDefaultRotations.Clear();
        }

        public void RegisterBone(string name, Transform boneTransform)
        {
            if (boneTransform == null || string.IsNullOrEmpty(name)) return;

            _bones[name] = boneTransform;
            _boneDefaultPositions[name] = boneTransform.localPosition;
            _boneDefaultRotations[name] = boneTransform.localRotation;
        }

        public void UnregisterBone(string name)
        {
            if (_bones.ContainsKey(name))
            {
                _bones.Remove(name);
                _boneDefaultPositions.Remove(name);
                _boneDefaultRotations.Remove(name);
            }
        }

        public string[] GetBoneList()
        {
            string[] keys = new string[_bones.Count];
            _bones.Keys.CopyTo(keys, 0);
            return keys;
        }

        public Transform GetBone(string name)
        {
            if (_bones.TryGetValue(name, out Transform bone))
            {
                return bone;
            }
            Debug.LogWarning($"[BoneController] Bone not found: {name}");
            return null;
        }

        public void RotateBone(string name, Quaternion rot)
        {
            if (_bones.TryGetValue(name, out Transform bone))
            {
                bone.localRotation = rot;
            }
        }

        public void TranslateBone(string name, Vector3 pos)
        {
            if (_bones.TryGetValue(name, out Transform bone))
            {
                bone.localPosition = pos;
            }
        }

        public void ResetBone(string name)
        {
            if (_bones.ContainsKey(name))
            {
                if (_boneDefaultPositions.TryGetValue(name, out Vector3 defaultPos))
                {
                    _bones[name].localPosition = defaultPos;
                }
                if (_boneDefaultRotations.TryGetValue(name, out Quaternion defaultRot))
                {
                    _bones[name].localRotation = defaultRot;
                }
            }
        }

        public void ResetAllBones()
        {
            foreach (var kvp in _bones)
            {
                ResetBone(kvp.Key);
            }
        }

        public bool HasBone(string name)
        {
            return _bones.ContainsKey(name);
        }

        public Vector3 GetBoneDefaultPosition(string name)
        {
            if (_boneDefaultPositions.TryGetValue(name, out Vector3 pos))
            {
                return pos;
            }
            return Vector3.zero;
        }

        public Quaternion GetBoneDefaultRotation(string name)
        {
            if (_boneDefaultRotations.TryGetValue(name, out Quaternion rot))
            {
                return rot;
            }
            return Quaternion.identity;
        }
    }
}