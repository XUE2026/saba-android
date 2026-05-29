using UnityEngine;

namespace Saba.MMDViewer
{
    public class CameraController : MonoBehaviour
    {
        [Header("Camera Settings")]
        [SerializeField] private Transform target;
        [SerializeField] private float distance = 5f;
        [SerializeField] private float minDistance = 1f;
        [SerializeField] private float maxDistance = 20f;
        [SerializeField] private float xSpeed = 120f;
        [SerializeField] private float ySpeed = 120f;
        [SerializeField] private float zoomSpeed = 2f;
        [SerializeField] private float damping = 5f;
        [SerializeField] private float minYAngle = -80f;
        [SerializeField] private float maxYAngle = 80f;

        private float currentX;
        private float currentY;
        private float currentDistance;
        private float targetX;
        private float targetY;
        private float targetDistance;

        private Vector3 targetPosition;
        private Vector3 desiredPosition;
        private Quaternion desiredRotation;

        private bool isDragging;
        private bool isPinching;
        private Vector2 lastTouchPosition;
        private float lastPinchDistance;

        private Camera cam;

        private void Awake()
        {
            cam = GetComponent<Camera>();
            if (cam == null)
            {
                cam = Camera.main;
            }

            currentDistance = distance;
            targetDistance = distance;
            currentX = 0f;
            currentY = 20f;
            targetX = currentX;
            targetY = currentY;
        }

        private void LateUpdate()
        {
            HandleInput();

            SmoothDamp();

            UpdateCameraTransform();

            LookAtTarget();
        }

        private void HandleInput()
        {
            HandleMouseInput();
            HandleTouchInput();
        }

        private void HandleMouseInput()
        {
            if (Input.GetMouseButtonDown(0))
            {
                isDragging = true;
                lastTouchPosition = Input.mousePosition;
            }

            if (Input.GetMouseButtonUp(0))
            {
                isDragging = false;
            }

            if (isDragging && Input.GetMouseButton(0))
            {
                Vector2 delta = (Vector2)Input.mousePosition - lastTouchPosition;

                if (delta.magnitude > 0.1f)
                {
                    targetX += delta.x * xSpeed * 0.005f;
                    targetY -= delta.y * ySpeed * 0.005f;
                    targetY = ClampAngle(targetY, minYAngle, maxYAngle);
                }

                lastTouchPosition = Input.mousePosition;
            }

            float scroll = Input.GetAxis("Mouse ScrollWheel");
            if (Mathf.Abs(scroll) > 0.01f)
            {
                targetDistance -= scroll * zoomSpeed;
                targetDistance = Mathf.Clamp(targetDistance, minDistance, maxDistance);
            }

            if (Input.GetMouseButtonDown(1))
            {
                isPinching = true;
                lastTouchPosition = Input.mousePosition;
            }

            if (Input.GetMouseButtonUp(1))
            {
                isPinching = false;
            }

            if (isPinching && Input.GetMouseButton(1))
            {
                Vector2 delta = (Vector2)Input.mousePosition - lastTouchPosition;
                if (target != null)
                {
                    Vector3 right = transform.right * -delta.x * 0.005f * distance;
                    Vector3 up = transform.up * -delta.y * 0.005f * distance;
                    targetPosition += right + up;

                    if (target != null)
                    {
                        target.position += right + up;
                    }
                }
                lastTouchPosition = Input.mousePosition;
            }
        }

        private void HandleTouchInput()
        {
            if (Input.touchCount == 1)
            {
                Touch touch = Input.GetTouch(0);

                switch (touch.phase)
                {
                    case TouchPhase.Began:
                        isDragging = true;
                        lastTouchPosition = touch.position;
                        break;

                    case TouchPhase.Moved:
                        if (isDragging)
                        {
                            Vector2 delta = touch.position - lastTouchPosition;
                            targetX += delta.x * xSpeed * 0.005f;
                            targetY -= delta.y * ySpeed * 0.005f;
                            targetY = ClampAngle(targetY, minYAngle, maxYAngle);
                            lastTouchPosition = touch.position;
                        }
                        break;

                    case TouchPhase.Ended:
                    case TouchPhase.Canceled:
                        isDragging = false;
                        break;
                }
            }
            else if (Input.touchCount == 2)
            {
                Touch touch0 = Input.GetTouch(0);
                Touch touch1 = Input.GetTouch(1);

                if (touch0.phase == TouchPhase.Began || touch1.phase == TouchPhase.Began)
                {
                    isPinching = true;
                    lastPinchDistance = Vector2.Distance(touch0.position, touch1.position);
                }
                else if (touch0.phase == TouchPhase.Moved || touch1.phase == TouchPhase.Moved)
                {
                    if (isPinching)
                    {
                        float currentPinchDistance = Vector2.Distance(touch0.position, touch1.position);
                        float delta = lastPinchDistance - currentPinchDistance;
                        targetDistance += delta * zoomSpeed * 0.01f;
                        targetDistance = Mathf.Clamp(targetDistance, minDistance, maxDistance);
                        lastPinchDistance = currentPinchDistance;
                    }

                    Vector2 touch0Delta = touch0.position - touch0.deltaPosition;
                    Vector2 touch1Delta = touch1.position - touch1.deltaPosition;
                    Vector2 midPoint = (touch0.position + touch1.position) * 0.5f;
                    Vector2 lastMidPoint = (touch0Delta + touch1Delta) * 0.5f;
                    Vector2 panDelta = midPoint - lastMidPoint;

                    if (panDelta.magnitude > 0.1f && target != null)
                    {
                        Vector3 right = transform.right * -panDelta.x * 0.005f * distance;
                        Vector3 up = transform.up * -panDelta.y * 0.005f * distance;
                        targetPosition += right + up;

                        target.position += right + up;
                    }
                }

                if (touch0.phase == TouchPhase.Ended || touch1.phase == TouchPhase.Ended ||
                    touch0.phase == TouchPhase.Canceled || touch1.phase == TouchPhase.Canceled)
                {
                    isPinching = false;
                }
            }
        }

        private void SmoothDamp()
        {
            currentX = Mathf.Lerp(currentX, targetX, Time.deltaTime * damping);
            currentY = Mathf.Lerp(currentY, targetY, Time.deltaTime * damping);
            currentDistance = Mathf.Lerp(currentDistance, targetDistance, Time.deltaTime * damping);
        }

        private void UpdateCameraTransform()
        {
            Vector3 direction = new Vector3(0, 0, -currentDistance);
            Quaternion rotation = Quaternion.Euler(currentY, currentX, 0);

            if (target != null)
            {
                desiredPosition = target.position + rotation * direction;
            }
            else
            {
                desiredPosition = rotation * direction;
            }

            desiredRotation = rotation;
        }

        private void LookAtTarget()
        {
            transform.position = desiredPosition;
            transform.rotation = desiredRotation;
        }

        public void SetTarget(Transform newTarget)
        {
            target = newTarget;
            targetPosition = newTarget != null ? newTarget.position : Vector3.zero;

            if (newTarget != null)
            {
                Vector3 relativePos = transform.position - newTarget.position;
                distance = relativePos.magnitude;
                targetDistance = distance;
                currentDistance = distance;

                Vector2 angles = CalculateAngles(relativePos);
                currentX = angles.x;
                currentY = angles.y;
                targetX = currentX;
                targetY = currentY;
            }
        }

        public void ResetView()
        {
            targetX = 0f;
            targetY = 20f;
            targetDistance = 5f;
            targetPosition = Vector3.zero;

            if (target != null)
            {
                target.position = Vector3.zero;
            }
        }

        public void SetDistance(float newDistance)
        {
            targetDistance = Mathf.Clamp(newDistance, minDistance, maxDistance);
        }

        public float GetDistance()
        {
            return currentDistance;
        }

        public Transform GetTarget()
        {
            return target;
        }

        public void SetAngles(float x, float y)
        {
            targetX = x;
            targetY = ClampAngle(y, minYAngle, maxYAngle);
        }

        public void SetDamping(float newDamping)
        {
            damping = Mathf.Max(0.1f, newDamping);
        }

        private Vector2 CalculateAngles(Vector3 relativePos)
        {
            float x = Mathf.Atan2(relativePos.x, relativePos.z) * Mathf.Rad2Deg;
            float y = Mathf.Asin(relativePos.y / relativePos.magnitude) * Mathf.Rad2Deg;
            return new Vector2(x, y);
        }

        private static float ClampAngle(float angle, float min, float max)
        {
            if (angle < -360f) angle += 360f;
            if (angle > 360f) angle -= 360f;
            return Mathf.Clamp(angle, min, max);
        }

        private void OnValidate()
        {
            minDistance = Mathf.Max(0.1f, minDistance);
            maxDistance = Mathf.Max(minDistance, maxDistance);
            minYAngle = Mathf.Clamp(minYAngle, -89f, 89f);
            maxYAngle = Mathf.Clamp(maxYAngle, -89f, 89f);
        }
    }
}