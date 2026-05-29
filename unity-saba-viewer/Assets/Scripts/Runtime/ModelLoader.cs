using System.IO;
using UnityEngine;

namespace Saba.MMDViewer
{
    public class ModelLoader : MonoBehaviour
    {
        private GameObject loadedModel;
        private Animator modelAnimator;

        public GameObject LoadModel(string path)
        {
            if (string.IsNullOrEmpty(path))
            {
                Debug.LogError("[ModelLoader] Path is null or empty");
                return null;
            }

            UnloadModel();

            if (!File.Exists(path))
            {
                Debug.LogError($"[ModelLoader] File not found: {path}");
                return null;
            }

            string extension = Path.GetExtension(path).ToLowerInvariant();

            switch (extension)
            {
                case ".fbx":
                    loadedModel = LoadFBX(path);
                    break;
                case ".pmx":
                    Debug.LogWarning("[ModelLoader] Native PMX loading is not supported. Attempting to load as FBX.");
                    loadedModel = LoadFBX(path);
                    break;
                default:
                    Debug.LogWarning($"[ModelLoader] Unsupported format: {extension}. Attempting FBX load.");
                    loadedModel = LoadFBX(path);
                    break;
            }

            if (loadedModel != null)
            {
                SetupModelComponents();
            }

            return loadedModel;
        }

        public GameObject LoadModelFromBundle(string bundlePath, string assetName)
        {
            UnloadModel();

            AssetBundle bundle = AssetBundle.LoadFromFile(bundlePath);
            if (bundle == null)
            {
                Debug.LogError($"[ModelLoader] Failed to load AssetBundle: {bundlePath}");
                return null;
            }

            GameObject prefab = bundle.LoadAsset<GameObject>(assetName);
            if (prefab == null)
            {
                Debug.LogError($"[ModelLoader] Asset '{assetName}' not found in bundle");
                bundle.Unload(false);
                return null;
            }

            loadedModel = Instantiate(prefab);
            loadedModel.name = prefab.name;
            bundle.Unload(false);

            if (loadedModel != null)
            {
                SetupModelComponents();
            }

            return loadedModel;
        }

        public GameObject LoadModelFromResources(string resourcePath)
        {
            UnloadModel();

            GameObject prefab = Resources.Load<GameObject>(resourcePath);
            if (prefab == null)
            {
                Debug.LogError($"[ModelLoader] Resource not found: {resourcePath}");
                return null;
            }

            loadedModel = Instantiate(prefab);
            loadedModel.name = prefab.name;

            if (loadedModel != null)
            {
                SetupModelComponents();
            }

            return loadedModel;
        }

        public void LoadModelFromBytes(byte[] data, string modelName)
        {
            UnloadModel();

            Debug.LogWarning("[ModelLoader] Direct model loading from bytes is not supported. FBX/PMX require Unity's import pipeline.");
        }

        private GameObject LoadFBX(string path)
        {
            string resourcesPath = GetResourcesRelativePath(path);
            if (!string.IsNullOrEmpty(resourcesPath))
            {
                GameObject prefab = Resources.Load<GameObject>(resourcesPath);
                if (prefab != null)
                {
                    GameObject instance = Instantiate(prefab);
                    instance.name = prefab.name;
                    return instance;
                }
            }

            AssetBundle bundle = AssetBundle.LoadFromFile(path);
            if (bundle != null)
            {
                GameObject[] assets = bundle.LoadAllAssets<GameObject>();
                if (assets.Length > 0)
                {
                    GameObject instance = Instantiate(assets[0]);
                    instance.name = assets[0].name;
                    bundle.Unload(false);
                    return instance;
                }
                bundle.Unload(false);
            }

            Debug.LogError($"[ModelLoader] Could not load FBX from: {path}. Place model in Resources folder or use AssetBundle.");
            return null;
        }

        private string GetResourcesRelativePath(string absolutePath)
        {
            string resourcesDir = Path.Combine(Application.dataPath, "Resources");
            if (absolutePath.StartsWith(resourcesDir))
            {
                string relative = absolutePath.Substring(resourcesDir.Length + 1);
                return Path.ChangeExtension(relative, null);
            }
            return null;
        }

        private void SetupModelComponents()
        {
            modelAnimator = loadedModel.GetComponent<Animator>();
            if (modelAnimator == null)
            {
                modelAnimator = loadedModel.GetComponentInChildren<Animator>();
            }

            if (modelAnimator == null)
            {
                modelAnimator = loadedModel.AddComponent<Animator>();
            }

            loadedModel.transform.localScale = Vector3.one;
            loadedModel.transform.position = Vector3.zero;
            loadedModel.transform.rotation = Quaternion.identity;

            SkinnedMeshRenderer[] renderers = loadedModel.GetComponentsInChildren<SkinnedMeshRenderer>();
            foreach (SkinnedMeshRenderer renderer in renderers)
            {
                renderer.updateWhenOffscreen = true;
            }
        }

        public void UnloadModel()
        {
            if (loadedModel != null)
            {
                Destroy(loadedModel);
                loadedModel = null;
                modelAnimator = null;
            }

            Resources.UnloadUnusedAssets();
        }

        public GameObject GetModelRoot()
        {
            return loadedModel;
        }

        public Animator GetModelAnimator()
        {
            return modelAnimator;
        }
    }
}