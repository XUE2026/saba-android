using System;
using System.Collections.Generic;
using UnityEngine;

namespace Saba.MMDViewer
{
    public enum GroundType
    {
        Floor,
        Grass,
        Mud,
        Asphalt,
        Sky,
        Field
    }

    [Serializable]
    public class GroundSettings
    {
        public Color color = Color.gray;
        public Material material;
    }

    public class EnvironmentManager : MonoBehaviour
    {
        [Header("Sun Settings")]
        [SerializeField] private Light sunLight;
        [SerializeField] private Vector3 sunDirection = new Vector3(50f, 30f, 0f);
        [SerializeField] private float sunIntensity = 1f;
        [SerializeField] private Color sunColor = Color.white;

        [Header("Ground Settings")]
        [SerializeField] private GroundType currentGroundType = GroundType.Floor;
        [SerializeField] private GameObject groundObject;
        [SerializeField] private Material groundMaterial;

        [Header("Ambient")]
        [SerializeField] private Color ambientColor = new Color(0.5f, 0.5f, 0.5f);

        private List<GameObject> primitives = new List<GameObject>();
        private MaterialPropertyBlock groundPropertyBlock;
        private Quaternion sunBaseRotation;

        private static readonly Dictionary<GroundType, Color> GroundColors = new Dictionary<GroundType, Color>
        {
            { GroundType.Floor, new Color(0.5f, 0.5f, 0.5f) },
            { GroundType.Grass, new Color(0.2f, 0.6f, 0.1f) },
            { GroundType.Mud, new Color(0.4f, 0.25f, 0.1f) },
            { GroundType.Asphalt, new Color(0.2f, 0.2f, 0.22f) },
            { GroundType.Sky, new Color(0.3f, 0.5f, 0.8f) },
            { GroundType.Field, new Color(0.35f, 0.3f, 0.15f) }
        };

        private void Awake()
        {
            if (sunLight == null)
            {
                GameObject sunObj = new GameObject("Sun Light");
                sunObj.transform.SetParent(transform);
                sunLight = sunObj.AddComponent<Light>();
                sunLight.type = LightType.Directional;
            }

            sunBaseRotation = Quaternion.Euler(sunDirection);
            sunLight.transform.rotation = sunBaseRotation;
            sunLight.intensity = sunIntensity;
            sunLight.color = sunColor;

            if (groundObject == null)
            {
                CreateDefaultGround();
            }

            groundPropertyBlock = new MaterialPropertyBlock();
            RenderSettings.ambientLight = ambientColor;

            SetGroundType(currentGroundType);
        }

        private void CreateDefaultGround()
        {
            groundObject = GameObject.CreatePrimitive(PrimitiveType.Plane);
            groundObject.name = "Ground";
            groundObject.transform.SetParent(transform);
            groundObject.transform.position = new Vector3(0f, -0.5f, 0f);
            groundObject.transform.localScale = new Vector3(10f, 1f, 10f);

            MeshRenderer renderer = groundObject.GetComponent<MeshRenderer>();
            if (renderer != null)
            {
                renderer.shadowCastingMode = UnityEngine.Rendering.ShadowCastingMode.ReceiveOnly;
                renderer.receiveShadows = true;
            }

            MeshCollider collider = groundObject.GetComponent<MeshCollider>();
            if (collider != null)
            {
                collider.enabled = true;
            }
        }

        public void SetSunDirection(Vector3 direction)
        {
            sunDirection = direction.normalized;
            if (sunLight != null)
            {
                sunLight.transform.rotation = Quaternion.LookRotation(sunDirection);
            }
        }

        public void SetSunIntensity(float intensity)
        {
            sunIntensity = Mathf.Max(0f, intensity);
            if (sunLight != null)
            {
                sunLight.intensity = sunIntensity;
            }
        }

        public void SetSunColor(Color color)
        {
            sunColor = color;
            if (sunLight != null)
            {
                sunLight.color = sunColor;
            }
        }

        public Vector3 GetSunDirection()
        {
            return sunDirection;
        }

        public float GetSunIntensity()
        {
            return sunIntensity;
        }

        public void SetGroundType(GroundType type)
        {
            currentGroundType = type;

            if (groundObject == null) return;

            MeshRenderer renderer = groundObject.GetComponent<MeshRenderer>();
            if (renderer == null) return;

            if (groundMaterial != null)
            {
                renderer.material = groundMaterial;
                if (GroundColors.TryGetValue(type, out Color color))
                {
                    renderer.material.color = color;
                }
            }
            else
            {
                groundPropertyBlock.Clear();
                if (GroundColors.TryGetValue(type, out Color color))
                {
                    groundPropertyBlock.SetColor("_Color", color);
                }
                renderer.SetPropertyBlock(groundPropertyBlock);
            }
        }

        public GroundType GetGroundType()
        {
            return currentGroundType;
        }

        public void AddPrimitive(PrimitiveType type, Vector3 position, Vector3 scale, Color color)
        {
            GameObject primitive = GameObject.CreatePrimitive(type);
            primitive.name = $"Primitive_{type}_{primitives.Count}";
            primitive.transform.SetParent(transform);
            primitive.transform.position = position;
            primitive.transform.localScale = scale;

            MeshRenderer renderer = primitive.GetComponent<MeshRenderer>();
            if (renderer != null)
            {
                Material material = new Material(Shader.Find("Standard"));
                material.color = color;
                renderer.material = material;
            }

            primitives.Add(primitive);
        }

        public void AddPrimitive(PrimitiveType type, Vector3 position, Quaternion rotation, Vector3 scale, Color color)
        {
            GameObject primitive = GameObject.CreatePrimitive(type);
            primitive.name = $"Primitive_{type}_{primitives.Count}";
            primitive.transform.SetParent(transform);
            primitive.transform.position = position;
            primitive.transform.rotation = rotation;
            primitive.transform.localScale = scale;

            MeshRenderer renderer = primitive.GetComponent<MeshRenderer>();
            if (renderer != null)
            {
                Material material = new Material(Shader.Find("Standard"));
                material.color = color;
                renderer.material = material;
            }

            primitives.Add(primitive);
        }

        public void ClearPrimitives()
        {
            foreach (GameObject primitive in primitives)
            {
                if (primitive != null)
                {
                    Destroy(primitive);
                }
            }
            primitives.Clear();
        }

        public void SetAmbientColor(Color color)
        {
            ambientColor = color;
            RenderSettings.ambientLight = ambientColor;
        }

        public void SetGroundMaterial(Material material)
        {
            groundMaterial = material;
            if (groundObject != null)
            {
                MeshRenderer renderer = groundObject.GetComponent<MeshRenderer>();
                if (renderer != null)
                {
                    renderer.material = material;
                }
            }
        }

        private void OnDestroy()
        {
            ClearPrimitives();

            if (groundObject != null)
            {
                Destroy(groundObject);
            }
        }
    }
}