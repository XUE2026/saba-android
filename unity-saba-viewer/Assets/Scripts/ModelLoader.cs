using System;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;
using UnityEngine;

namespace UnitySabaViewer
{
    public class ModelLoader : MonoBehaviour
    {
        private static ModelLoader _instance;
        public static ModelLoader Instance
        {
            get
            {
                if (_instance == null)
                {
                    _instance = FindObjectOfType<ModelLoader>();
                    if (_instance == null)
                    {
                        GameObject go = new GameObject("ModelLoader");
                        _instance = go.AddComponent<ModelLoader>();
                    }
                }
                return _instance;
            }
        }

        private GameObject _loadedModel;
        private string _loadedPath;
        private readonly Dictionary<string, GameObject> _modelCache = new Dictionary<string, GameObject>();

        public GameObject LoadedModel => _loadedModel;
        public bool IsModelLoaded => _loadedModel != null;

        public async Task<GameObject> LoadModel(string path)
        {
            if (string.IsNullOrEmpty(path))
            {
                Debug.LogError("[ModelLoader] Path is null or empty");
                return null;
            }

            if (_modelCache.ContainsKey(path))
            {
                _loadedModel = Instantiate(_modelCache[path]);
                _loadedPath = path;
                Debug.Log($"[ModelLoader] Loaded from cache: {path}");
                return _loadedModel;
            }

            string fullPath = path;
            if (!File.Exists(fullPath))
            {
                fullPath = Path.Combine(Application.streamingAssetsPath, path);
            }

            if (!File.Exists(fullPath) && !fullPath.Contains("://"))
            {
                Debug.LogError($"[ModelLoader] File not found: {fullPath}");
                return null;
            }

            try
            {
                byte[] data = await Task.Run(() => File.ReadAllBytes(fullPath));
                string extension = Path.GetExtension(fullPath).ToLowerInvariant();

                GameObject modelRoot = new GameObject(Path.GetFileNameWithoutExtension(path));
                modelRoot.transform.position = Vector3.zero;

                if (extension == ".pmx" || extension == ".pmd")
                {
                    SetupMMDModel(modelRoot, data, extension);
                }
                else
                {
                    Debug.LogWarning($"[ModelLoader] Unsupported format: {extension}");
                    Destroy(modelRoot);
                    return null;
                }

                _modelCache[path] = modelRoot;
                _loadedModel = Instantiate(modelRoot);
                _loadedPath = path;

                Debug.Log($"[ModelLoader] Loaded model: {path}");
                return _loadedModel;
            }
            catch (Exception ex)
            {
                Debug.LogError($"[ModelLoader] Failed to load model: {ex.Message}");
                return null;
            }
        }

        private void SetupMMDModel(GameObject root, byte[] data, string extension)
        {
            GameObject meshContainer = new GameObject("Mesh");
            meshContainer.transform.SetParent(root.transform);

            GameObject bonesContainer = new GameObject("Bones");
            bonesContainer.transform.SetParent(root.transform);

            GameObject materialsContainer = new GameObject("Materials");
            materialsContainer.transform.SetParent(root.transform);

            BoneController boneCtrl = root.AddComponent<BoneController>();
            boneCtrl.Initialize(bonesContainer.transform);

            MeshFilter meshFilter = meshContainer.AddComponent<MeshFilter>();
            MeshRenderer meshRenderer = meshContainer.AddComponent<MeshRenderer>();

            Mesh mesh = new Mesh();
            mesh.name = $"{root.name}_mesh";
            meshFilter.mesh = mesh;

            Material defaultMat = new Material(Shader.Find("Universal Render Pipeline/Lit"));
            meshRenderer.material = defaultMat;
        }

        public void UnloadModel()
        {
            if (_loadedModel != null)
            {
                Destroy(_loadedModel);
                _loadedModel = null;
                _loadedPath = null;
                Resources.UnloadUnusedAssets();
                Debug.Log("[ModelLoader] Model unloaded");
            }
        }

        public GameObject GetModel()
        {
            return _loadedModel;
        }

        public void ClearCache()
        {
            foreach (var kvp in _modelCache)
            {
                if (kvp.Value != null)
                {
                    Destroy(kvp.Value);
                }
            }
            _modelCache.Clear();
            Debug.Log("[ModelLoader] Cache cleared");
        }

        private void OnDestroy()
        {
            UnloadModel();
            ClearCache();
            if (_instance == this)
            {
                _instance = null;
            }
        }
    }
}