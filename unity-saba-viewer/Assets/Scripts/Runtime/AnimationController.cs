using System;
using UnityEngine;

namespace Saba.MMDViewer
{
    public enum AnimationType
    {
        Idle,
        Walk,
        Run,
        Jump
    }

    public class AnimationController : MonoBehaviour
    {
        [Header("Animation Settings")]
        [SerializeField] private Animator targetAnimator;
        [SerializeField] private AnimationType currentAnimation = AnimationType.Idle;
        [SerializeField] private float animationSpeed = 1f;
        [SerializeField] private float transitionDuration = 0.25f;

        private bool isPlaying;
        private float currentTime;
        private float idleShiftTimer;
        private bool isGrounded = true;

        private static readonly int SpeedParam = Animator.StringToHash("Speed");
        private static readonly int IsMovingParam = Animator.StringToHash("IsMoving");
        private static readonly int IsRunningParam = Animator.StringToHash("IsRunning");
        private static readonly int JumpParam = Animator.StringToHash("Jump");
        private static readonly int IsGroundedParam = Animator.StringToHash("IsGrounded");
        private static readonly int MotionSpeedParam = Animator.StringToHash("MotionSpeed");

        public void SetAnimator(Animator animator)
        {
            targetAnimator = animator;
            isPlaying = false;
            currentTime = 0f;

            if (targetAnimator != null)
            {
                targetAnimator.speed = animationSpeed;
            }
        }

        public void PlayIdle()
        {
            if (targetAnimator == null) return;

            currentAnimation = AnimationType.Idle;
            isPlaying = true;

            targetAnimator.SetFloat(SpeedParam, 0f);
            targetAnimator.SetBool(IsMovingParam, false);
            targetAnimator.SetBool(IsRunningParam, false);
            targetAnimator.SetBool(IsGroundedParam, true);
            targetAnimator.SetFloat(MotionSpeedParam, 1f);

            ResetTriggers();
        }

        public void PlayWalk()
        {
            if (targetAnimator == null) return;

            currentAnimation = AnimationType.Walk;
            isPlaying = true;
            isGrounded = true;

            targetAnimator.SetFloat(SpeedParam, 0.5f);
            targetAnimator.SetBool(IsMovingParam, true);
            targetAnimator.SetBool(IsRunningParam, false);
            targetAnimator.SetBool(IsGroundedParam, true);
            targetAnimator.SetFloat(MotionSpeedParam, 1f);

            ResetTriggers();
        }

        public void PlayRun()
        {
            if (targetAnimator == null) return;

            currentAnimation = AnimationType.Run;
            isPlaying = true;
            isGrounded = true;

            targetAnimator.SetFloat(SpeedParam, 1f);
            targetAnimator.SetBool(IsMovingParam, true);
            targetAnimator.SetBool(IsRunningParam, true);
            targetAnimator.SetBool(IsGroundedParam, true);
            targetAnimator.SetFloat(MotionSpeedParam, 1.5f);

            ResetTriggers();
        }

        public void PlayJump()
        {
            if (targetAnimator == null) return;

            currentAnimation = AnimationType.Jump;
            isPlaying = true;
            isGrounded = false;

            targetAnimator.SetTrigger(JumpParam);
            targetAnimator.SetBool(IsGroundedParam, false);
            targetAnimator.SetBool(IsMovingParam, false);
            targetAnimator.SetFloat(SpeedParam, 0f);

            StartCoroutine(HandleJump());
        }

        private System.Collections.IEnumerator HandleJump()
        {
            yield return new WaitForSeconds(0.5f);

            if (isPlaying && currentAnimation == AnimationType.Jump)
            {
                isGrounded = true;
                targetAnimator.SetBool(IsGroundedParam, true);
                PlayIdle();
            }
        }

        public void StopAll()
        {
            isPlaying = false;
            currentTime = 0f;

            if (targetAnimator == null) return;

            targetAnimator.SetFloat(SpeedParam, 0f);
            targetAnimator.SetBool(IsMovingParam, false);
            targetAnimator.SetBool(IsRunningParam, false);
            targetAnimator.SetBool(IsGroundedParam, true);
            targetAnimator.SetFloat(MotionSpeedParam, 0f);

            ResetTriggers();

            targetAnimator.speed = 0f;
        }

        public void SetAnimationSpeed(float speed)
        {
            animationSpeed = Mathf.Max(0f, speed);
            if (targetAnimator != null)
            {
                targetAnimator.speed = animationSpeed;
            }
        }

        public float GetAnimationSpeed()
        {
            return animationSpeed;
        }

        public AnimationType GetCurrentAnimation()
        {
            return currentAnimation;
        }

        public bool IsPlaying()
        {
            return isPlaying;
        }

        public bool IsGrounded()
        {
            return isGrounded;
        }

        private void ResetTriggers()
        {
            if (targetAnimator == null) return;
            targetAnimator.ResetTrigger(JumpParam);
        }

        private void Update()
        {
            if (!isPlaying || targetAnimator == null) return;

            targetAnimator.speed = animationSpeed;

            if (currentAnimation == AnimationType.Idle)
            {
                idleShiftTimer += Time.deltaTime;
                if (idleShiftTimer > 5f)
                {
                    targetAnimator.SetFloat("IdleOffset", UnityEngine.Random.Range(0f, 1f));
                    idleShiftTimer = 0f;
                }
            }
        }

        private void OnAnimatorMove()
        {
            if (targetAnimator == null || !isPlaying) return;

            if (currentAnimation == AnimationType.Walk || currentAnimation == AnimationType.Run)
            {
                Vector3 deltaPosition = targetAnimator.deltaPosition;
                deltaPosition.y = 0f;
                targetAnimator.transform.position += deltaPosition;
            }
        }
    }
}