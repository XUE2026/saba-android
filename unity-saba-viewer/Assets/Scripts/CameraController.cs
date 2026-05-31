using UnityEngine;

namespace UnitySabaViewer
{
    public class CameraController : MonoBehaviour
    {
        [Header("Settings")]
        [SerializeField] private float _minDistance = 0.5f;
        [SerializeField] private float _maxDistance = 20f;
        [SerializeField] private float _defaultDistance = 5f;
        [SerializeField] private float _rotationSpeed = 4f;
        [SerializeField] private float _zoomSpeed = 2f;
        [SerializeField] private float _smoothSpeed = 8f;

        private Transform _cameraTransform;
        private float _currentDistance;
        private float _targetDistance;
        private float _currentRotationX;
        private float _targetRotationX;
        private float _currentRotationY;
        private float _targetRotationY;
        private Vector3 _targetPosition;
        private Vector2 _lastTouchPosition;
        private float _lastPinchDistance;
        private bool _isDragging;

        public float distance
        {
            get => _currentDistance;
            set => _targetDistance = Mathf.Clamp(value, _minDistance, _maxDistance);
        }

        public float rotationX
        {
            get => _currentRotationX;
            set => _targetRotationX = value;
        }

        public float rotationY
        {
            get => _currentRotationY;
            set => _targetRotationY = value;
        }

        public Transform target
        {
            get => _targetPosition != Vector3.zero ? null : null;
            set
            {
                if (value != null)
                {
                    _targetPosition = value.position;
                }
            }
        }

        public Vector3 TargetPosition
        {
            get => _targetPosition;
            set => _targetPosition = value;
        }

        private void Awake()
        {
            _cameraTransform = transform;
            _currentDistance = _defaultDistance;
            _targetDistance = _defaultDistance;
            _targetPosition = Vector3.zero;
        }

        private void LateUpdate()
        {
            HandleTouchInput();

            _currentDistance = Mathf.Lerp(_currentDistance, _targetDistance, Time.deltaTime * _smoothSpeed);
            _currentRotationX = Mathf.Lerp(_currentRotationX, _targetRotationX, Time.deltaTime * _smoothSpeed);
            _currentRotationY = Mathf.Lerp(_currentRotationY, _targetRotationY, Time.deltaTime * _smoothSpeed);

            Quaternion rotation = Quaternion.Euler(_currentRotationY, _currentRotationX, 0);
            Vector3 position = _targetPosition - rotation * Vector3.forward * _currentDistance;

            _cameraTransform.rotation = rotation;
            _cameraTransform.position = position;
        }

        private void HandleTouchInput()
        {
            if (Input.touchCount == 1)
            {
                Touch touch = Input.GetTouch(0);

                switch (touch.phase)
                {
                    case TouchPhase.Began:
                        _isDragging = true;
                        _lastTouchPosition = touch.position;
                        break;

                    case TouchPhase.Moved:
                        if (_isDragging)
                        {
                            Vector2 delta = touch.position - _lastTouchPosition;
                            _targetRotationX += delta.x * _rotationSpeed * Time.deltaTime;
                            _targetRotationY -= delta.y * _rotationSpeed * Time.deltaTime;
                            _targetRotationY = Mathf.Clamp(_targetRotationY, -89f, 89f);
                            _lastTouchPosition = touch.position;
                        }
                        break;

                    case TouchPhase.Ended:
                    case TouchPhase.Canceled:
                        _isDragging = false;
                        break;
                }
            }
            else if (Input.touchCount == 2)
            {
                Touch touch0 = Input.GetTouch(0);
                Touch touch1 = Input.GetTouch(1);

                if (touch0.phase == TouchPhase.Began || touch1.phase == TouchPhase.Began)
                {
                    _lastPinchDistance = Vector2.Distance(touch0.position, touch1.position);
                }
                else
                {
                    float currentPinchDistance = Vector2.Distance(touch0.position, touch1.position);
                    float delta = (_lastPinchDistance - currentPinchDistance) * _zoomSpeed * Time.deltaTime;
                    _targetDistance = Mathf.Clamp(_targetDistance + delta, _minDistance, _maxDistance);
                    _lastPinchDistance = currentPinchDistance;
                }
            }

            if (Input.GetMouseButton(0))
            {
                float mouseX = Input.GetAxis("Mouse X");
                float mouseY = Input.GetAxis("Mouse Y");

                if (Mathf.Abs(mouseX) > 0.01f || Mathf.Abs(mouseY) > 0.01f)
                {
                    _targetRotationX += mouseX * _rotationSpeed;
                    _targetRotationY -= mouseY * _rotationSpeed;
                    _targetRotationY = Mathf.Clamp(_targetRotationY, -89f, 89f);
                }
            }

            float scroll = Input.GetAxis("Mouse ScrollWheel");
            if (Mathf.Abs(scroll) > 0.01f)
            {
                _targetDistance = Mathf.Clamp(_targetDistance - scroll * _zoomSpeed, _minDistance, _maxDistance);
            }
        }

        public void ResetCamera()
        {
            _targetDistance = _defaultDistance;
            _targetRotationX = 0;
            _targetRotationY = 0;
            _targetPosition = Vector3.zero;
        }

        public void SetDistance(float dist)
        {
            distance = dist;
        }

        public void SetAngle(float x, float y)
        {
            _targetRotationX = x;
            _targetRotationY = Mathf.Clamp(y, -89f, 89f);
        }
    }
}