using System;
using System.Collections.Generic;
using UnityEngine;

namespace Saba.MMDViewer
{
    [Serializable]
    public class BoneInfo
    {
        public string name;
        public HumanBodyBones humanBone;
        public Transform boneTransform;
        public Vector3 initialPosition;
        public Quaternion initialRotation;
        public bool isValid;

        public BoneInfo(string name, HumanBodyBones humanBone)
        {
            this.name = name;
            this.humanBone = humanBone;
            this.isValid = false;
        }
    }

    public class BoneController : MonoBehaviour
    {
        [Header("Bone Settings")]
        [SerializeField] private Animator targetAnimator;
        [SerializeField] private string selectedBoneName = string.Empty;

        private Dictionary<string, BoneInfo> boneMap = new Dictionary<string, BoneInfo>();
        private BoneInfo selectedBone;
        private Transform selectedBoneTransform;
        private Quaternion selectedBoneInitialRotation;
        private Dictionary<Transform, Quaternion> initialRotations = new Dictionary<Transform, Quaternion>();

        private static readonly HumanBodyBones[] StandardBones = new HumanBodyBones[]
        {
            HumanBodyBones.Hips,
            HumanBodyBones.Spine,
            HumanBodyBones.Chest,
            HumanBodyBones.UpperChest,
            HumanBodyBones.Neck,
            HumanBodyBones.Head,
            HumanBodyBones.LeftEye,
            HumanBodyBones.RightEye,
            HumanBodyBones.Jaw,
            HumanBodyBones.LeftShoulder,
            HumanBodyBones.RightShoulder,
            HumanBodyBones.LeftUpperArm,
            HumanBodyBones.RightUpperArm,
            HumanBodyBones.LeftLowerArm,
            HumanBodyBones.RightLowerArm,
            HumanBodyBones.LeftHand,
            HumanBodyBones.RightHand,
            HumanBodyBones.LeftUpperLeg,
            HumanBodyBones.RightUpperLeg,
            HumanBodyBones.LeftLowerLeg,
            HumanBodyBones.RightLowerLeg,
            HumanBodyBones.LeftFoot,
            HumanBodyBones.RightFoot,
            HumanBodyBones.LeftToes,
            HumanBodyBones.RightToes,
            HumanBodyBones.LeftThumbProximal,
            HumanBodyBones.LeftThumbIntermediate,
            HumanBodyBones.LeftThumbDistal,
            HumanBodyBones.RightThumbProximal,
            HumanBodyBones.RightThumbIntermediate,
            HumanBodyBones.RightThumbDistal,
            HumanBodyBones.LeftIndexProximal,
            HumanBodyBones.LeftIndexIntermediate,
            HumanBodyBones.LeftIndexDistal,
            HumanBodyBones.RightIndexProximal,
            HumanBodyBones.RightIndexIntermediate,
            HumanBodyBones.RightIndexDistal,
            HumanBodyBones.LeftMiddleProximal,
            HumanBodyBones.LeftMiddleIntermediate,
            HumanBodyBones.LeftMiddleDistal,
            HumanBodyBones.RightMiddleProximal,
            HumanBodyBones.RightMiddleIntermediate,
            HumanBodyBones.RightMiddleDistal,
            HumanBodyBones.LeftRingProximal,
            HumanBodyBones.LeftRingIntermediate,
            HumanBodyBones.LeftRingDistal,
            HumanBodyBones.RightRingProximal,
            HumanBodyBones.RightRingIntermediate,
            HumanBodyBones.RightRingDistal,
            HumanBodyBones.LeftLittleProximal,
            HumanBodyBones.LeftLittleIntermediate,
            HumanBodyBones.LeftLittleDistal,
            HumanBodyBones.RightLittleProximal,
            HumanBodyBones.RightLittleIntermediate,
            HumanBodyBones.RightLittleDistal
        };

        public void SetAnimator(Animator animator)
        {
            targetAnimator = animator;
            boneMap.Clear();
            initialRotations.Clear();
            selectedBone = null;
            selectedBoneTransform = null;
            selectedBoneName = string.Empty;

            if (targetAnimator == null || !targetAnimator.isHuman) return;

            BuildBoneMap();
        }

        private void BuildBoneMap()
        {
            foreach (HumanBodyBones humanBone in StandardBones)
            {
                Transform boneTransform = targetAnimator.GetBoneTransform(humanBone);
                if (boneTransform == null) continue;

                string boneName = boneTransform.name;
                BoneInfo info = new BoneInfo(boneName, humanBone)
                {
                    boneTransform = boneTransform,
                    initialPosition = boneTransform.localPosition,
                    initialRotation = boneTransform.localRotation,
                    isValid = true
                };

                boneMap[boneName] = info;
                initialRotations[boneTransform] = boneTransform.localRotation;
            }
        }

        public List<string> GetBoneList()
        {
            List<string> boneNames = new List<string>();
            foreach (var kvp in boneMap)
            {
                boneNames.Add(kvp.Key);
            }
            return boneNames;
        }

        public List<BoneInfo> GetBoneInfoList()
        {
            List<BoneInfo> boneInfos = new List<BoneInfo>();
            foreach (var kvp in boneMap)
            {
                boneInfos.Add(kvp.Value);
            }
            return boneInfos;
        }

        public bool SelectBone(string name)
        {
            if (string.IsNullOrEmpty(name) || !boneMap.TryGetValue(name, out BoneInfo info))
            {
                selectedBone = null;
                selectedBoneTransform = null;
                selectedBoneName = string.Empty;
                return false;
            }

            selectedBone = info;
            selectedBoneTransform = info.boneTransform;
            selectedBoneName = name;
            selectedBoneInitialRotation = info.initialRotation;
            return true;
        }

        public string GetSelectedBoneName()
        {
            return selectedBoneName;
        }

        public BoneInfo GetSelectedBone()
        {
            return selectedBone;
        }

        public void RotateBone(Vector3 eulerAngles)
        {
            if (selectedBoneTransform == null || selectedBone == null) return;

            Quaternion deltaRotation = Quaternion.Euler(eulerAngles);
            selectedBoneTransform.localRotation = selectedBoneInitialRotation * deltaRotation;
        }

        public void SetBoneRotation(Quaternion rotation)
        {
            if (selectedBoneTransform == null || selectedBone == null) return;

            selectedBoneTransform.localRotation = rotation;
        }

        public Quaternion GetBoneRotation(string name)
        {
            if (boneMap.TryGetValue(name, out BoneInfo info) && info.boneTransform != null)
            {
                return info.boneTransform.localRotation;
            }
            return Quaternion.identity;
        }

        public Vector3 GetBonePosition(string name)
        {
            if (boneMap.TryGetValue(name, out BoneInfo info) && info.boneTransform != null)
            {
                return info.boneTransform.localPosition;
            }
            return Vector3.zero;
        }

        public void ResetBone()
        {
            if (selectedBone == null || selectedBoneTransform == null) return;

            selectedBoneTransform.localPosition = selectedBone.initialPosition;
            selectedBoneTransform.localRotation = selectedBone.initialRotation;
        }

        public void ResetAllBones()
        {
            foreach (var kvp in boneMap)
            {
                if (kvp.Value.boneTransform != null)
                {
                    kvp.Value.boneTransform.localPosition = kvp.Value.initialPosition;
                    kvp.Value.boneTransform.localRotation = kvp.Value.initialRotation;
                }
            }
        }

        public bool HasAnimator()
        {
            return targetAnimator != null && targetAnimator.isHuman;
        }

        public int GetBoneCount()
        {
            return boneMap.Count;
        }
    }
}