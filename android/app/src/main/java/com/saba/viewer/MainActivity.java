package com.saba.viewer;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

public class MainActivity extends AppCompatActivity {

    private static final int PERMISSION_REQUEST_CODE = 100;
    private static final int FILE_PICK_MODEL = 1001;
    private static final int FILE_PICK_MOTION = 1002;
    private static final int FILE_PICK_SCENE = 1003;
    private static final int FILE_PICK_GOBOT = 1004;

    private NativeGLSurfaceView glSurfaceView;
    private boolean nativeInitialized = false;
    private int currentOrientation = Configuration.ORIENTATION_PORTRAIT;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        glSurfaceView = findViewById(R.id.gl_surface_view);
        glSurfaceView.setMainActivity(this);

        checkPermissions();
        handleIntent(getIntent());

        currentOrientation = getResources().getConfiguration().orientation;
        onOrientationChanged(currentOrientation);
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        handleIntent(intent);
    }

    private void handleIntent(Intent intent) {
        if (intent != null && intent.getData() != null) {
            Uri uri = intent.getData();
            String path = uri.getPath();
            if (path != null) {
                if (path.endsWith(".pmx") || path.endsWith(".pmd") ||
                    path.endsWith(".fbx") || path.endsWith(".obj")) {
                    loadModelFromUri(uri);
                } else if (path.endsWith(".vmd") || path.endsWith(".gobot")) {
                    loadMotionFromUri(uri);
                }
            }
        }
    }

    private void checkPermissions() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            String[] permissions = {
                Manifest.permission.READ_MEDIA_IMAGES,
                Manifest.permission.READ_MEDIA_VIDEO
            };
            boolean allGranted = true;
            for (String perm : permissions) {
                if (ContextCompat.checkSelfPermission(this, perm) != PackageManager.PERMISSION_GRANTED) {
                    allGranted = false;
                    break;
                }
            }
            if (!allGranted) {
                ActivityCompat.requestPermissions(this, permissions, PERMISSION_REQUEST_CODE);
            }
        } else {
            String[] permissions = {
                Manifest.permission.READ_EXTERNAL_STORAGE,
                Manifest.permission.WRITE_EXTERNAL_STORAGE
            };
            boolean allGranted = true;
            for (String perm : permissions) {
                if (ContextCompat.checkSelfPermission(this, perm) != PackageManager.PERMISSION_GRANTED) {
                    allGranted = false;
                    break;
                }
            }
            if (!allGranted) {
                ActivityCompat.requestPermissions(this, permissions, PERMISSION_REQUEST_CODE);
            }
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == PERMISSION_REQUEST_CODE) {
            for (int result : grantResults) {
                if (result != PackageManager.PERMISSION_GRANTED) {
                    Toast.makeText(this, R.string.settings, Toast.LENGTH_LONG).show();
                    return;
                }
            }
        }
    }

    public void openModelPicker() {
        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        intent.setType("*/*");
        String[] mimeTypes = {
            "application/octet-stream",
            "model/obj",
            "model/fbx",
            "image/bmp",
            "image/png",
            "image/jpeg"
        };
        intent.putExtra(Intent.EXTRA_MIME_TYPES, mimeTypes);
        startActivityForResult(intent, FILE_PICK_MODEL);
    }

    public void openMotionPicker() {
        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        intent.setType("*/*");
        startActivityForResult(intent, FILE_PICK_MOTION);
    }

    public void openScenePicker() {
        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        intent.setType("*/*");
        startActivityForResult(intent, FILE_PICK_SCENE);
    }

    public void openGobotPicker() {
        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        intent.setType("*/*");
        startActivityForResult(intent, FILE_PICK_GOBOT);
    }

    public void openSettings() {
        Intent intent = new Intent(this, SettingsActivity.class);
        startActivity(intent);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (resultCode == RESULT_OK && data != null) {
            Uri uri = data.getData();
            if (uri != null) {
                switch (requestCode) {
                    case FILE_PICK_MODEL:
                        loadModelFromUri(uri);
                        break;
                    case FILE_PICK_MOTION:
                        loadMotionFromUri(uri);
                        break;
                    case FILE_PICK_SCENE:
                        loadSceneFromUri(uri);
                        break;
                    case FILE_PICK_GOBOT:
                        loadGobotFromUri(uri);
                        break;
                }
            }
        }
    }

    private void loadModelFromUri(Uri uri) {
        String path = copyUriToCache(uri, "model");
        if (path != null) {
            glSurfaceView.queueEvent(() -> nativeLoadModel(path));
            Toast.makeText(this, getString(R.string.load_model) + ": " + uri.getLastPathSegment(), Toast.LENGTH_SHORT).show();
        }
    }

    private void loadMotionFromUri(Uri uri) {
        String path = copyUriToCache(uri, "motion");
        if (path != null) {
            glSurfaceView.queueEvent(() -> nativeLoadMotion(path));
            Toast.makeText(this, getString(R.string.load_motion) + ": " + uri.getLastPathSegment(), Toast.LENGTH_SHORT).show();
        }
    }

    private void loadSceneFromUri(Uri uri) {
        String path = copyUriToCache(uri, "scene");
        if (path != null) {
            glSurfaceView.queueEvent(() -> nativeLoadScene(path));
        }
    }

    private void loadGobotFromUri(Uri uri) {
        String path = copyUriToCache(uri, "gobot");
        if (path != null) {
            glSurfaceView.queueEvent(() -> nativeLoadGobot(path));
        }
    }

    private String copyUriToCache(Uri uri, String prefix) {
        try {
            String fileName = prefix + "_" + System.currentTimeMillis();
            File cacheFile = new File(getCacheDir(), fileName);
            try (InputStream is = getContentResolver().openInputStream(uri);
                 FileOutputStream os = new FileOutputStream(cacheFile)) {
                byte[] buffer = new byte[8192];
                int len;
                while ((len = is.read(buffer)) != -1) {
                    os.write(buffer, 0, len);
                }
            }
            return cacheFile.getAbsolutePath();
        } catch (Exception e) {
            e.printStackTrace();
            runOnUiThread(() -> Toast.makeText(this, "Error: " + e.getMessage(), Toast.LENGTH_LONG).show());
            return null;
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (glSurfaceView != null) {
            glSurfaceView.onPause();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (glSurfaceView != null) {
            glSurfaceView.onResume();
        }
    }

    @Override
    protected void onDestroy() {
        if (glSurfaceView != null) {
            glSurfaceView.queueEvent(() -> nativeDestroy());
        }
        super.onDestroy();
    }

    @Override
    public void onConfigurationChanged(@NonNull Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        if (newConfig.orientation != currentOrientation) {
            currentOrientation = newConfig.orientation;
            onOrientationChanged(newConfig.orientation);
        }
    }

    private void onOrientationChanged(int orientation) {
        if (glSurfaceView != null) {
            boolean isLandscape = orientation == Configuration.ORIENTATION_LANDSCAPE;
            glSurfaceView.queueEvent(() -> nativeSetOrientation(isLandscape));
        }
    }

    // Native methods
    private static native void nativeInit(String assetPath);
    private static native void nativeResize(int width, int height);
    private static native void nativeStep();
    private static native void nativeDestroy();
    private static native void nativeTouchEvent(int pointerId, float x, float y, int action);
    private static native void nativeLoadModel(String path);
    private static native void nativeLoadMotion(String path);
    private static native void nativeLoadScene(String path);
    private static native void nativeLoadGobot(String path);
    private static native void nativeSetOrientation(boolean isLandscape);
    private static native void nativeSetFilter(int filterIndex);
    private static native void nativeSetFilterParam(int paramIndex, float value);
    private static native void nativeSetEnvironmentParam(int paramIndex, float value);
    private static native void nativeSetBoneTransform(int boneIndex, float x, float y, float z, float rx, float ry, float rz);
    private static native void nativePlayBuiltinAction(int actionIndex);
    private static native void nativeStartRecording();
    private static native void nativeStopRecording(String outputPath);
    private static native void nativeSetScreenOrientation(int orientation);

    static {
        System.loadLibrary("SabaViewer");
    }
}