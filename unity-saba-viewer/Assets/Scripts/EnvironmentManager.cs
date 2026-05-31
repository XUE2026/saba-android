using UnityEngine;

namespace UnitySabaViewer
{
    public enum GroundType
    {
        Grid,
        Checker,
        Grass,
        Dirt,
        Asphalt
    }

    public enum PrimitiveType
    {
        Cube,
        Sphere,
        Cone,
        Cylinder
    }

    public class EnvironmentManager : MonoBehaviour
    {
        public static EnvironmentManager Instance { get; private set; }

        [Header("Sun/Light")]
        [SerializeField] private Light _directionalLight;

        [Header("Ground")]
        [SerializeField] private GameObject _groundObject;
        [SerializeField] private GroundType _currentGroundType = GroundType.Grid;

        [Header("Primitives")]
        [SerializeField] private Transform _primitivesContainer;

        private Material _groundMaterial;

        private void Awake()
        {
            if (Instance != null && Instance != this)
            {
                Destroy(this);
                return;
            }
            Instance = this;

            if (_directionalLight == null)
            {
                GameObject lightObj = GameObject.Find("Directional Light");
                if (lightObj == null)
                {
                    lightObj = new GameObject("Directional Light");
                    _directionalLight = lightObj.AddComponent<Light>();
                    _directionalLight.type = LightType.Directional;
                }
                else
                {
                    _directionalLight = lightObj.GetComponent<Light>();
                }
            }

            if (_groundObject == null)
            {
                _groundObject = GameObject.CreatePrimitive(UnityEngine.PrimitiveType.Plane);
                _groundObject.name = "Ground";
                _groundObject.transform.position = new Vector3(0, -0.5f, 0);
                _groundObject.transform.localScale = new Vector3(10, 1, 10);
            }

            if (_primitivesContainer == null)
            {
                GameObject container = new GameObject("Primitives");
                _primitivesContainer = container.transform;
            }

            _groundMaterial = new Material(Shader.Find("Universal Render Pipeline/Lit"));
            _groundObject.GetComponent<MeshRenderer>().material = _groundMaterial;

            SetGround(_currentGroundType);
            SetupDefaultSun();
        }

        private void SetupDefaultSun()
        {
            if (_directionalLight != null)
            {
                _directionalLight.transform.rotation = Quaternion.Euler(50f, -30f, 0);
                _directionalLight.intensity = 1.0f;
                _directionalLight.color = Color.white;
            }
        }

        public void SetSun(float dirX, float dirY, float colorTemp)
        {
            if (_directionalLight == null) return;

            _directionalLight.transform.rotation = Quaternion.Euler(dirY, dirX, 0);
            Color color = Mathf.CorrelatedColorTemperatureToRGB(colorTemp);
            _directionalLight.color = color;
        }

        public void SetSunDirection(float dirX, float dirY)
        {
            if (_directionalLight == null) return;
            _directionalLight.transform.rotation = Quaternion.Euler(dirY, dirX, 0);
        }

        public void SetSunIntensity(float intensity)
        {
            if (_directionalLight == null) return;
            _directionalLight.intensity = Mathf.Max(0, intensity);
        }

        public void SetSunColor(float colorTemp)
        {
            if (_directionalLight == null) return;
            _directionalLight.color = Mathf.CorrelatedColorTemperatureToRGB(colorTemp);
        }

        public void SetGround(int type)
        {
            if (type < 0 || type >= System.Enum.GetValues(typeof(GroundType)).Length)
            {
                Debug.LogWarning($"[EnvironmentManager] Invalid ground type: {type}");
                return;
            }

            _currentGroundType = (GroundType)type;
            ApplyGroundTexture();
        }

        private void ApplyGroundTexture()
        {
            if (_groundMaterial == null) return;

            Color color = Color.white;
            Texture2D tex = null;

            switch (_currentGroundType)
            {
                case GroundType.Grid:
                    color = new Color(0.8f, 0.8f, 0.8f);
                    tex = CreateGridTexture(512, 512, Color.gray, new Color(0.3f, 0.3f, 0.3f));
                    break;
                case GroundType.Checker:
                    color = Color.white;
                    tex = CreateCheckerTexture(512, 512, Color.white, new Color(0.2f, 0.2f, 0.2f));
                    break;
                case GroundType.Grass:
                    color = new Color(0.3f, 0.6f, 0.2f);
                    break;
                case GroundType.Dirt:
                    color = new Color(0.5f, 0.35f, 0.2f);
                    break;
                case GroundType.Asphalt:
                    color = new Color(0.25f, 0.25f, 0.28f);
                    break;
            }

            _groundMaterial.color = color;
            if (tex != null)
            {
                _groundMaterial.mainTexture = tex;
            }
        }

        private Texture2D CreateGridTexture(int width, int height, Color lineColor, Color bgColor)
        {
            Texture2D tex = new Texture2D(width, height);
            Color[] pixels = new Color[width * height];

            int gridSize = 32;
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    bool isLine = (x % gridSize == 0) || (y % gridSize == 0);
                    pixels[y * width + x] = isLine ? lineColor : bgColor;
                }
            }

            tex.SetPixels(pixels);
            tex.wrapMode = TextureWrapMode.Repeat;
            tex.Apply();
            return tex;
        }

        private Texture2D CreateCheckerTexture(int width, int height, Color color1, Color color2)
        {
            Texture2D tex = new Texture2D(width, height);
            Color[] pixels = new Color[width * height];

            int cellSize = 32;
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    bool isEven = ((x / cellSize) + (y / cellSize)) % 2 == 0;
                    pixels[y * width + x] = isEven ? color1 : color2;
                }
            }

            tex.SetPixels(pixels);
            tex.wrapMode = TextureWrapMode.Repeat;
            tex.Apply();
            return tex;
        }

        public void AddPrimitive(int type, Vector3 pos)
        {
            if (type < 0 || type >= System.Enum.GetValues(typeof(PrimitiveType)).Length)
            {
                Debug.LogWarning($"[EnvironmentManager] Invalid primitive type: {type}");
                return;
            }

            PrimitiveType primType = (PrimitiveType)type;
            UnityEngine.PrimitiveType unityPrim;

            switch (primType)
            {
                case PrimitiveType.Cube:
                    unityPrim = UnityEngine.PrimitiveType.Cube;
                    break;
                case PrimitiveType.Sphere:
                    unityPrim = UnityEngine.PrimitiveType.Sphere;
                    break;
                case PrimitiveType.Cone:
                case PrimitiveType.Cylinder:
                    unityPrim = UnityEngine.PrimitiveType.Cylinder;
                    break;
                default:
                    unityPrim = UnityEngine.PrimitiveType.Cube;
                    break;
            }

            GameObject prim = GameObject.CreatePrimitive(unityPrim);
            prim.name = primType.ToString();
            prim.transform.position = pos;
            prim.transform.SetParent(_primitivesContainer);

            if (primType == PrimitiveType.Cone)
            {
                prim.transform.localScale = new Vector3(0.5f, 1f, 0.5f);
            }
        }

        public void ClearPrimitives()
        {
            for (int i = _primitivesContainer.childCount - 1; i >= 0; i--)
            {
                Destroy(_primitivesContainer.GetChild(i).gameObject);
            }
        }

        public GroundType GetCurrentGroundType()
        {
            return _currentGroundType;
        }
    }
}